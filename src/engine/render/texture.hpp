// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_TEXTURE_HPP
#define CINDER_CITY_TEXTURE_HPP

#include <string>

struct SDL_GPUDevice;
struct SDL_GPUTexture;
struct SDL_GPUSampler;

namespace cinder {
    class graphics_device;

    // A 2D texture loaded from an image file (decoded to RGBA8), plus its sampler.
    // Uploads to VRAM on construction, releases everything on destruction.
    class texture {
    public:
        texture(const graphics_device&, const std::string&);
        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;

        ~texture();

        [[nodiscard]] SDL_GPUTexture* native() const noexcept { return texture_; }
        [[nodiscard]] SDL_GPUSampler* sampler() const noexcept { return sampler_; }

    private:
        SDL_GPUDevice*  device_  {nullptr};
        SDL_GPUTexture* texture_ {nullptr};
        SDL_GPUSampler* sampler_ {nullptr};
    };
}

#endif //CINDER_CITY_TEXTURE_HPP
