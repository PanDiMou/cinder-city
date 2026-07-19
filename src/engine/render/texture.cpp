// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "engine/render/texture.hpp"
#include "engine/render/graphics_device.hpp"

#include <SDL3/SDL.h>
#include <stb_image.h>   // pour décoder les PNG

#include <cstdint>
#include <cstring>       // std::memcpy
#include <stdexcept>
#include <string>

namespace cinder {
    texture::texture(const graphics_device& device, const std::string& path)
        : device_ {device.native()} {

        // 1) Décode l'image en pixels bruts. Le "4" force le format RGBA (4 canaux :
        // rouge, vert, bleu, alpha) même si l'image d'origine en a moins.
        // stbi_load remplit width/height/channels et renvoie un tableau d'octets.
        int width {0};
        int height {0};
        int channels {0};
        stbi_uc* pixels {stbi_load(path.c_str(), &width, &height, &channels, 4)};
        if (pixels == nullptr) {
            throw std::runtime_error("texture(" + path + ") : " + stbi_failure_reason());
        }

        const auto texture_width {static_cast<std::uint32_t>(width)};
        const auto texture_height {static_cast<std::uint32_t>(height)};
        // Taille totale en octets = largeur x hauteur x 4 (octets par pixel).
        const std::uint32_t byte_size {texture_width * texture_height * 4};

        // 2) Crée la texture côté GPU (encore vide). On décrit son format, sa taille,
        // et son usage (SAMPLER = elle sera lue par un shader).
        SDL_GPUTextureCreateInfo texture_info {};
        texture_info.type = SDL_GPU_TEXTURETYPE_2D;
        texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;  // 4 x 8 bits, normalisé 0..1
        texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        texture_info.width = texture_width;
        texture_info.height = texture_height;
        texture_info.layer_count_or_depth = 1;
        texture_info.num_levels = 1;   // pas de mipmaps

        texture_ = SDL_CreateGPUTexture(device_, &texture_info);
        if (texture_ == nullptr) {
            stbi_image_free(pixels);   // toujours libérer les pixels avant de sortir
            throw std::runtime_error(std::string{"Échec de SDL_CreateGPUTexture : "} + SDL_GetError());
        }

        // 3) Buffer de transfert (pont CPU -> GPU), comme pour les gpu_buffer.
        const SDL_GPUTransferBufferCreateInfo transfer_info {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = byte_size
        };
        SDL_GPUTransferBuffer* transfer {SDL_CreateGPUTransferBuffer(device_, &transfer_info)};
        if (transfer == nullptr) {
            stbi_image_free(pixels);
            SDL_ReleaseGPUTexture(device_, texture_);
            throw std::runtime_error(std::string{"Échec de SDL_CreateGPUTransferBuffer : "} + SDL_GetError());
        }

        // 4) On copie les pixels dans le buffer de transfert, puis on libère la
        // copie RAM de stb (elle ne sert plus). ORDRE important : copier AVANT de libérer.
        void* mapped {SDL_MapGPUTransferBuffer(device_, transfer, false)};
        std::memcpy(mapped, pixels, byte_size);
        SDL_UnmapGPUTransferBuffer(device_, transfer);
        stbi_image_free(pixels);

        // 5) Passe de copie : buffer de transfert (RAM) -> texture (VRAM).
        SDL_GPUCommandBuffer* command_buffer {SDL_AcquireGPUCommandBuffer(device_)};
        SDL_GPUCopyPass* copy_pass {SDL_BeginGPUCopyPass(command_buffer)};

        const SDL_GPUTextureTransferInfo source {
            .transfer_buffer = transfer,
            .offset = 0,
            .pixels_per_row = texture_width,    // sert au GPU pour découper les lignes
            .rows_per_layer = texture_height
        };
        const SDL_GPUTextureRegion destination {
            .texture = texture_,
            .w = texture_width,
            .h = texture_height,
            .d = 1
        };
        SDL_UploadToGPUTexture(copy_pass, &source, &destination, false);

        SDL_EndGPUCopyPass(copy_pass);
        SDL_SubmitGPUCommandBuffer(command_buffer);
        SDL_ReleaseGPUTransferBuffer(device_, transfer);

        // 6) Le SAMPLER = les règles de lecture de la texture.
        // NEAREST (au plus proche) : pas de mélange entre pixels voisins. C'est le
        // bon choix pour une palette Synty (cases de couleur plates), sinon les
        // couleurs "baveraient" à la frontière des cases.
        // CLAMP_TO_EDGE : si une coordonnée sort de [0,1], on reste bloqué au bord
        // (au lieu de répéter l'image en boucle).
        SDL_GPUSamplerCreateInfo sampler_info {};
        sampler_info.min_filter = SDL_GPU_FILTER_NEAREST;
        sampler_info.mag_filter = SDL_GPU_FILTER_NEAREST;
        sampler_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
        sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

        sampler_ = SDL_CreateGPUSampler(device_, &sampler_info);
        if (sampler_ == nullptr) {
            SDL_ReleaseGPUTexture(device_, texture_);
            throw std::runtime_error(std::string{"Échec de SDL_CreateGPUSampler : "} + SDL_GetError());
        }
    }

    // Destructeur : on libère le sampler puis la texture (ordre inverse de création).
    texture::~texture() {
        if (sampler_ != nullptr) {
            SDL_ReleaseGPUSampler(device_, sampler_);
        }
        if (texture_ != nullptr) {
            SDL_ReleaseGPUTexture(device_, texture_);
        }
    }
}
