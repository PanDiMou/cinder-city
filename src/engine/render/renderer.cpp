// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "engine/render/renderer.hpp"
#include "engine/render/graphics_device.hpp"
#include "engine/render/shader.hpp"
#include "engine/render/gpu_mesh.hpp"
#include "engine/render/mesh.hpp"
#include "engine/scene/camera.hpp"
#include "engine/world/world.hpp"
#include "engine/world/entity.hpp"
#include "engine/editor/ui.hpp"
#include "engine/core/log.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <cstddef>     // offsetof
#include <stdexcept>
#include <string>

namespace cinder {
    namespace {
        // Format du buffer de profondeur : 32 bits flottants pour la distance.
        constexpr auto depth_format {SDL_GPU_TEXTUREFORMAT_D32_FLOAT};
    }

    // ---- CONSTRUCTEUR : prépare tout ce qu'il faut pour dessiner ----
    renderer::renderer(const graphics_device& device)
        : device_ {device.native()},
          window_ {device.window_handle()},
          palette_ {device, "assets/Textures/Alts/Generic_01_A.png"},          // atlas de couleur des Background
          emissive_ {device, "assets/Textures/Emissive/Generic_Emissive_01_A.png"} { // émissif (inutilisé par les Background)

        // Le buffer de profondeur : une image invisible qui stocke, pour chaque
        // pixel, la distance de l'objet le plus proche déjà dessiné. C'est ce qui
        // évite qu'un objet lointain s'affiche par-dessus un objet proche.
        int width {0};
        int height {0};
        SDL_GetWindowSizeInPixels(window_, &width, &height);

        SDL_GPUTextureCreateInfo depth_info {};
        depth_info.type = SDL_GPU_TEXTURETYPE_2D;
        depth_info.format = depth_format;
        depth_info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
        depth_info.width = static_cast<Uint32>(width);
        depth_info.height = static_cast<Uint32>(height);
        depth_info.layer_count_or_depth = 1;
        depth_info.num_levels = 1;
        depth_info.sample_count = SDL_GPU_SAMPLECOUNT_1;

        depth_ = SDL_CreateGPUTexture(device_, &depth_info);
        if (depth_ == nullptr) {
            throw std::runtime_error(std::string{"Échec de SDL_CreateGPUTexture (depth) : "} + SDL_GetError());
        }

        // Décrit COMMENT lire un sommet dans le buffer : "pitch" = taille d'un
        // sommet (on avance de sizeof(vertex) octets pour passer au suivant).
        constexpr SDL_GPUVertexBufferDescription vertex_buffer {
            .slot = 0,
            .pitch = sizeof(vertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0
        };
        // Décrit les CHAMPS d'un sommet et où les trouver. "offsetof" donne la
        // position (en octets) d'un champ dans la struct. Ces "location" doivent
        // correspondre aux "layout(location = X)" dans les shaders.
        constexpr SDL_GPUVertexAttribute vertex_attributes[] {
            {   // location 0 : position (vec3)
                .location = 0,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .offset = offsetof(vertex, position)
            },
            {   // location 1 : coordonnées de texture (vec2)
                .location = 1,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .offset = offsetof(vertex, uv)
            }
        };
        // Le format de l'image finale (celle affichée à l'écran, la "swapchain").
        const SDL_GPUColorTargetDescription color_target {
            .format = SDL_GetGPUSwapchainTextureFormat(device_, window_)
        };

        // "Lambda" = une petite fonction anonyme définie sur place. Le "[&]" veut
        // dire qu'elle peut utiliser les variables locales d'au-dessus (par référence).
        // Elle fabrique un pipeline à partir de 2 shaders ; tout le reste est commun.
        const auto make_pipeline = [&](const char* vertex_path, const char* fragment_path) {
            const shader vertex_shader {device, vertex_path, shader_stage::vertex};
            const shader fragment_shader {device, fragment_path, shader_stage::fragment};

            SDL_GPUGraphicsPipelineCreateInfo info {};
            info.vertex_shader = vertex_shader.native();
            info.fragment_shader = fragment_shader.native();
            info.vertex_input_state.vertex_buffer_descriptions = &vertex_buffer;
            info.vertex_input_state.num_vertex_buffers = 1;
            info.vertex_input_state.vertex_attributes = vertex_attributes;
            info.vertex_input_state.num_vertex_attributes = 2;
            info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;      // on dessine des triangles
            info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;       // triangles pleins (pas fil de fer)
            info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;       // on dessine les 2 faces
            info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
            info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;  // garde le plus proche
            info.depth_stencil_state.enable_depth_test = true;            // teste la profondeur
            info.depth_stencil_state.enable_depth_write = true;
            info.target_info.color_target_descriptions = &color_target;
            info.target_info.num_color_targets = 1;
            info.target_info.depth_stencil_format = depth_format;
            info.target_info.has_depth_stencil_target = true;

            SDL_GPUGraphicsPipeline* pipeline {SDL_CreateGPUGraphicsPipeline(device_, &info)};
            if (pipeline == nullptr) {
                throw std::runtime_error(std::string{"Échec de SDL_CreateGPUGraphicsPipeline : "} + SDL_GetError());
            }
            return pipeline;
        };

        // On crée un pipeline par matériau, chacun avec sa paire de shaders.
        solid_color_pipeline_ = make_pipeline("shaders/solid_color.vert.spv", "shaders/solid_color.frag.spv");
        grid_floor_pipeline_ = make_pipeline("shaders/grid_floor.vert.spv", "shaders/grid_floor.frag.spv");
        textured_pipeline_ = make_pipeline("shaders/textured.vert.spv", "shaders/textured.frag.spv");
    }

    // ---- DESTRUCTEUR : libère les ressources (ordre inverse) ----
    renderer::~renderer() {
        if (depth_ != nullptr) {
            SDL_ReleaseGPUTexture(device_, depth_);
        }
        if (textured_pipeline_ != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, textured_pipeline_);
        }
        if (grid_floor_pipeline_ != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, grid_floor_pipeline_);
        }
        if (solid_color_pipeline_ != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, solid_color_pipeline_);
        }
        // palette_ (un objet texture) se libère toute seule : c'est du RAII.
    }

    // ---- DRAW : dessine une frame complète ----
    void renderer::draw(const camera& camera, const world& world, ui& ui) const {
        ui.finalize(); // termine la frame ImGui construite juste avant (côté application)

        // Un "command buffer" = une liste de commandes qu'on prépare, puis qu'on
        // envoie au GPU d'un coup. On commence par en obtenir un.
        SDL_GPUCommandBuffer* command_buffer {SDL_AcquireGPUCommandBuffer(device_)};
        if (command_buffer == nullptr) {
            log::error("Échec de SDL_AcquireGPUCommandBuffer : {}", SDL_GetError());
            return;
        }

        // La "swapchain" = l'image dans laquelle on dessine et qui sera affichée.
        // On l'attend et on la récupère (elle change à chaque frame).
        SDL_GPUTexture* swapchain {nullptr};
        Uint32 width {0};
        Uint32 height {0};
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, window_, &swapchain, &width, &height)) {
            log::warning("Échec de SDL_WaitAndAcquireGPUSwapchainTexture : {}", SDL_GetError());
            SDL_SubmitGPUCommandBuffer(command_buffer);
            return;
        }
        if (swapchain == nullptr) {
            SDL_SubmitGPUCommandBuffer(command_buffer); // fenêtre minimisée : rien à dessiner
            return;
        }

        // On prépare les données de l'UI (ses sommets/indices) AVANT d'ouvrir une passe.
        ui.upload(command_buffer);

        // La matrice caméra, calculée pour le ratio actuel de la fenêtre.
        const float aspect {static_cast<float>(width) / static_cast<float>(height)};
        const glm::mat4 view_projection {camera.view_projection(aspect)};

        // Cible couleur : on efface l'écran en bleu ciel (CLEAR) puis on garde le résultat (STORE).
        SDL_GPUColorTargetInfo color {};
        color.texture = swapchain;
        color.clear_color = SDL_FColor {0.53f, 0.81f, 0.92f, 1.0f}; // bleu ciel
        color.load_op = SDL_GPU_LOADOP_CLEAR;
        color.store_op = SDL_GPU_STOREOP_STORE;

        // Cible profondeur : on l'efface à 1.0 (le plus loin) au début de la frame.
        SDL_GPUDepthStencilTargetInfo depth {};
        depth.texture = depth_;
        depth.clear_depth = 1.0f;
        depth.load_op = SDL_GPU_LOADOP_CLEAR;
        depth.store_op = SDL_GPU_STOREOP_DONT_CARE;

        // On ouvre la "passe de rendu" : à l'intérieur, on dessine le monde.
        SDL_GPURenderPass* pass {SDL_BeginGPURenderPass(command_buffer, &color, 1, &depth)};

        // Pour chaque entité du monde, on dessine avec le pipeline de son matériau.
        for (const auto& entity : world.entities()) {
            const material_type material {entity->material()};

            // On choisit le bon pipeline. Le switch part de solid_color par défaut.
            SDL_GPUGraphicsPipeline* pipeline {solid_color_pipeline_};
            switch (material) {
                case material_type::grid_floor:  pipeline = grid_floor_pipeline_; break;
                case material_type::textured:    pipeline = textured_pipeline_;   break;
                case material_type::solid_color: break;   // garde le défaut
            }
            SDL_BindGPUGraphicsPipeline(pass, pipeline);

            // Matrice "MVP" = caméra x transform de l'objet. On l'envoie au shader
            // sommet (slot 0) : c'est elle qui place l'objet à l'écran.
            const glm::mat4 model_view_projection {view_projection * entity->get_transform().matrix()};
            SDL_PushGPUVertexUniformData(command_buffer, 0, &model_view_projection, sizeof(model_view_projection));

            // Selon le matériau, on fournit au shader fragment la bonne ressource :
            if (material == material_type::textured) {
                // Deux textures pour les modèles : la palette (couleur) et l'émissif
                // (lumière des fenêtres). Le shader fait palette + émissif.
                const SDL_GPUTextureSamplerBinding bindings[2] {
                    {.texture = palette_.native(),  .sampler = palette_.sampler()},
                    {.texture = emissive_.native(), .sampler = emissive_.sampler()}
                };
                SDL_BindGPUFragmentSamplers(pass, 0, bindings, 2);
            } else {
                // ...soit une couleur unie pour solid_color / grid_floor.
                const glm::vec4 color_uniform {entity->color()};
                SDL_PushGPUFragmentUniformData(command_buffer, 0, &color_uniform, sizeof(color_uniform));
            }

            // Enfin, on dessine la géométrie de l'entité.
            entity->mesh().bind_and_draw(pass);
        }

        SDL_EndGPURenderPass(pass);   // fin de la passe "monde"

        // Deuxième passe, juste pour l'UI : on GARDE (LOAD) l'image du monde et on
        // dessine l'interface par-dessus. Pas de profondeur ici (l'UI est plate, 2D).
        SDL_GPUColorTargetInfo ui_color {};
        ui_color.texture = swapchain;
        ui_color.load_op = SDL_GPU_LOADOP_LOAD;
        ui_color.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* ui_pass {SDL_BeginGPURenderPass(command_buffer, &ui_color, 1, nullptr)};
        ui.render(command_buffer, ui_pass);
        SDL_EndGPURenderPass(ui_pass);

        // On envoie toutes les commandes au GPU : c'est là que le dessin a lieu.
        SDL_SubmitGPUCommandBuffer(command_buffer);
    }
}
