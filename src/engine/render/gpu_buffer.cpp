// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/render/gpu_buffer.hpp"
#include "engine/render/graphics_device.hpp"

#include <SDL3/SDL.h>
#include <cstring>
#include <stdexcept>
#include <string>

namespace cinder {
    gpu_buffer::gpu_buffer(const graphics_device& device, const void* data,
                           const std::uint32_t size, const buffer_usage usage)
        : device_ {device.native()} {

        const SDL_GPUBufferUsageFlags usage_flags {
            usage == buffer_usage::vertex ? SDL_GPU_BUFFERUSAGE_VERTEX : SDL_GPU_BUFFERUSAGE_INDEX
        };

        const SDL_GPUBufferCreateInfo buffer_info {.usage = usage_flags, .size = size};
        buffer_ = SDL_CreateGPUBuffer(device_, &buffer_info);
        if (buffer_ == nullptr) {
            throw std::runtime_error(std::string{"SDL_CreateGPUBuffer: "} + SDL_GetError());
        }

        // Staging buffer, CPU-visible, used to copy the data over to the GPU buffer.
        const SDL_GPUTransferBufferCreateInfo transfer_info {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = size
        };
        SDL_GPUTransferBuffer* transfer {SDL_CreateGPUTransferBuffer(device_, &transfer_info)};
        if (transfer == nullptr) {
            SDL_ReleaseGPUBuffer(device_, buffer_);
            throw std::runtime_error(std::string{"SDL_CreateGPUTransferBuffer: "} + SDL_GetError());
        }

        void* mapped {SDL_MapGPUTransferBuffer(device_, transfer, false)};
        std::memcpy(mapped, data, size);
        SDL_UnmapGPUTransferBuffer(device_, transfer);

        // Copy pass: transfer buffer (RAM) -> GPU buffer (VRAM).
        SDL_GPUCommandBuffer* command_buffer {SDL_AcquireGPUCommandBuffer(device_)};
        SDL_GPUCopyPass* copy_pass {SDL_BeginGPUCopyPass(command_buffer)};

        const SDL_GPUTransferBufferLocation source {.transfer_buffer = transfer, .offset = 0};
        const SDL_GPUBufferRegion destination {.buffer = buffer_, .offset = 0, .size = size};
        SDL_UploadToGPUBuffer(copy_pass, &source, &destination, false);

        SDL_EndGPUCopyPass(copy_pass);
        SDL_SubmitGPUCommandBuffer(command_buffer);

        SDL_ReleaseGPUTransferBuffer(device_, transfer);
    }

    gpu_buffer::~gpu_buffer() {
        if (buffer_ != nullptr) {
            SDL_ReleaseGPUBuffer(device_, buffer_);
        }
    }
}
