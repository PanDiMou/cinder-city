// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/render/gpu_buffer.hpp"
#include "engine/render/graphics_device.hpp"

#include <SDL3/SDL.h>
#include <cstring>     // std::memcpy (copie d'octets)
#include <stdexcept>
#include <string>

namespace cinder {
    gpu_buffer::gpu_buffer(const graphics_device& device, const void* data,
                           const std::uint32_t size, const buffer_usage usage)
        : device_ {device.native()} {

        // Traduit notre enum vers le drapeau SDL correspondant (sommets ou indices).
        const SDL_GPUBufferUsageFlags usage_flags {
            usage == buffer_usage::vertex ? SDL_GPU_BUFFERUSAGE_VERTEX : SDL_GPU_BUFFERUSAGE_INDEX
        };

        // 1) Crée le buffer de destination, DANS la VRAM (encore vide).
        // ".usage = ..." est une "initialisation désignée" : on nomme les champs
        // qu'on remplit, c'est plus lisible.
        const SDL_GPUBufferCreateInfo buffer_info {.usage = usage_flags, .size = size};
        buffer_ = SDL_CreateGPUBuffer(device_, &buffer_info);
        if (buffer_ == nullptr) {
            throw std::runtime_error(std::string{"Échec de SDL_CreateGPUBuffer : "} + SDL_GetError());
        }

        // 2) Crée un buffer "de transfert", visible du CPU. Le GPU ne peut pas lire
        // directement notre RAM : ce buffer sert de pont intermédiaire.
        const SDL_GPUTransferBufferCreateInfo transfer_info {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = size
        };
        SDL_GPUTransferBuffer* transfer {SDL_CreateGPUTransferBuffer(device_, &transfer_info)};
        if (transfer == nullptr) {
            SDL_ReleaseGPUBuffer(device_, buffer_);   // on nettoie ce qu'on a déjà créé
            throw std::runtime_error(std::string{"Échec de SDL_CreateGPUTransferBuffer : "} + SDL_GetError());
        }

        // 3) On "mappe" le buffer de transfert : on obtient un pointeur CPU dessus,
        // on y recopie nos données (memcpy), puis on le "démappe".
        void* mapped {SDL_MapGPUTransferBuffer(device_, transfer, false)};
        std::memcpy(mapped, data, size);
        SDL_UnmapGPUTransferBuffer(device_, transfer);

        // 4) "Passe de copie" : on ordonne au GPU de copier le buffer de transfert
        // (RAM) vers le buffer final (VRAM). On enregistre la commande, puis on la soumet.
        SDL_GPUCommandBuffer* command_buffer {SDL_AcquireGPUCommandBuffer(device_)};
        SDL_GPUCopyPass* copy_pass {SDL_BeginGPUCopyPass(command_buffer)};

        const SDL_GPUTransferBufferLocation source {.transfer_buffer = transfer, .offset = 0};
        const SDL_GPUBufferRegion destination {.buffer = buffer_, .offset = 0, .size = size};
        SDL_UploadToGPUBuffer(copy_pass, &source, &destination, false);

        SDL_EndGPUCopyPass(copy_pass);
        SDL_SubmitGPUCommandBuffer(command_buffer);

        // 5) Le buffer de transfert a fini son travail : on le libère. La donnée
        // vit désormais dans buffer_ (VRAM).
        SDL_ReleaseGPUTransferBuffer(device_, transfer);
    }

    gpu_buffer::~gpu_buffer() {
        if (buffer_ != nullptr) {
            SDL_ReleaseGPUBuffer(device_, buffer_);
        }
    }
}
