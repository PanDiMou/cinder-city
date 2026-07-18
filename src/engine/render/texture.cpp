// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/render/texture.hpp"
#include "engine/render/graphics_device.hpp"

#include <SDL3/SDL.h>
#include <stb_image.h>

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

namespace cinder {
    texture::texture(const graphics_device& device, const std::string& path)
        : device_ {device.native()} {

        // Decode the file to RGBA8 (forced 4 channels, whatever the source has).
        int width {0};
        int height {0};
        int channels {0};
        stbi_uc* pixels {stbi_load(path.c_str(), &width, &height, &channels, 4)};
        if (pixels == nullptr) {
            throw std::runtime_error("texture(" + path + "): " + stbi_failure_reason());
        }

        const auto texture_width {static_cast<std::uint32_t>(width)};
        const auto texture_height {static_cast<std::uint32_t>(height)};
        const std::uint32_t byte_size {texture_width * texture_height * 4};

        // GPU texture, sampled from a shader.
        SDL_GPUTextureCreateInfo texture_info {};
        texture_info.type = SDL_GPU_TEXTURETYPE_2D;
        texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        texture_info.width = texture_width;
        texture_info.height = texture_height;
        texture_info.layer_count_or_depth = 1;
        texture_info.num_levels = 1;

        texture_ = SDL_CreateGPUTexture(device_, &texture_info);
        if (texture_ == nullptr) {
            stbi_image_free(pixels);
            throw std::runtime_error(std::string{"SDL_CreateGPUTexture: "} + SDL_GetError());
        }

        // Staging buffer, CPU-visible, to copy the pixels over to the GPU texture.
        const SDL_GPUTransferBufferCreateInfo transfer_info {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = byte_size
        };
        SDL_GPUTransferBuffer* transfer {SDL_CreateGPUTransferBuffer(device_, &transfer_info)};
        if (transfer == nullptr) {
            stbi_image_free(pixels);
            SDL_ReleaseGPUTexture(device_, texture_);
            throw std::runtime_error(std::string{"SDL_CreateGPUTransferBuffer: "} + SDL_GetError());
        }

        void* mapped {SDL_MapGPUTransferBuffer(device_, transfer, false)};
        std::memcpy(mapped, pixels, byte_size);
        SDL_UnmapGPUTransferBuffer(device_, transfer);
        stbi_image_free(pixels);

        // Copy pass: transfer buffer (RAM) -> GPU texture (VRAM).
        SDL_GPUCommandBuffer* command_buffer {SDL_AcquireGPUCommandBuffer(device_)};
        SDL_GPUCopyPass* copy_pass {SDL_BeginGPUCopyPass(command_buffer)};

        const SDL_GPUTextureTransferInfo source {
            .transfer_buffer = transfer,
            .offset = 0,
            .pixels_per_row = texture_width,
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

        // Sampler: how the shader reads texels. Nearest filtering keeps the flat
        // palette cells crisp (no color bleeding between neighboring swatches).
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
            throw std::runtime_error(std::string{"SDL_CreateGPUSampler: "} + SDL_GetError());
        }
    }

    texture::~texture() {
        if (sampler_ != nullptr) {
            SDL_ReleaseGPUSampler(device_, sampler_);
        }
        if (texture_ != nullptr) {
            SDL_ReleaseGPUTexture(device_, texture_);
        }
    }
}
