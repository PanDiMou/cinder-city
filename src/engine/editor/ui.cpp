// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/editor/ui.hpp"
#include "engine/render/graphics_device.hpp"

// ImGui = le "cœur" ; imgui_impl_sdl3 = le pont vers les entrées SDL ;
// imgui_impl_sdlgpu3 = le pont vers notre rendu SDL_gpu.
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

#include <SDL3/SDL.h>

namespace cinder {
    ui::ui(const graphics_device& device)
        : device_ {device.native()} {

        IMGUI_CHECKVERSION();          // vérifie que header et lib sont compatibles
        ImGui::CreateContext();        // crée l'état global d'ImGui
        ImGui::StyleColorsDark();      // thème sombre

        // On branche les deux backends : SDL3 (entrées) et SDL_gpu (rendu).
        ImGui_ImplSDL3_InitForSDLGPU(device.window_handle());

        ImGui_ImplSDLGPU3_InitInfo init {};
        init.Device = device_;
        // ImGui doit dessiner dans le même format d'image que l'écran.
        init.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(device_, device.window_handle());
        init.MSAASamples = SDL_GPU_SAMPLECOUNT_1;   // pas d'anti-aliasing multi-échantillons
        ImGui_ImplSDLGPU3_Init(&init);
    }

    ui::~ui() {
        // On démonte dans l'ordre inverse de l'initialisation.
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    // Donne un évènement (clic, touche...) à ImGui pour qu'il puisse réagir.
    void ui::process_event(const SDL_Event& event) {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    // Active ou coupe la souris pour l'UI. En mode vol (souris capturée par la
    // caméra), on la COUPE ici avec le drapeau NoMouse : sinon un clic maintenu
    // pourrait quand même toucher une fenêtre ImGui. À appeler AVANT begin_frame,
    // car ImGui lit ce drapeau au début de la frame.
    void ui::set_mouse_enabled(const bool enabled) {
        ImGuiIO& io {ImGui::GetIO()};
        if (enabled) {
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;   // enlève le drapeau -> souris active
        } else {
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse;    // ajoute le drapeau -> souris ignorée
        }
    }

    // Démarre une nouvelle frame ImGui. Après ça, on peut appeler ImGui::Begin, etc.
    void ui::begin_frame() {
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    // Termine la frame : ImGui calcule la liste des formes 2D à dessiner (mais ne
    // dessine encore rien sur le GPU).
    void ui::finalize() {
        ImGui::Render();
    }

    // Envoie les sommets/indices de l'UI vers le GPU. À faire AVANT d'ouvrir la
    // passe de rendu où on dessinera l'UI.
    void ui::upload(SDL_GPUCommandBuffer* command_buffer) {
        ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), command_buffer);
    }

    // Dessine réellement l'UI dans la passe de rendu fournie.
    void ui::render(SDL_GPUCommandBuffer* command_buffer, SDL_GPURenderPass* pass) {
        ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), command_buffer, pass);
    }

    // WantCaptureMouse = ImGui nous dit "cette souris est pour moi" (curseur au-dessus
    // d'une fenêtre, ou en train d'y cliquer). On s'en sert pour éviter de poser un
    // bâtiment quand l'utilisateur clique en réalité sur un panneau.
    bool ui::wants_mouse() const {
        return ImGui::GetIO().WantCaptureMouse;
    }
}
