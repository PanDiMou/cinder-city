// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_RENDERER_HPP
#define CINDER_CITY_RENDERER_HPP

#include "engine/render/gpu_buffer.hpp"

#include <cstdint>

struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUTexture;

namespace cinder {
    class graphics_device;
    class camera;
    struct mesh;

    // Owns the graphics pipeline and the GPU geometry, and draws one frame.
    class renderer {
    public:
        renderer(const graphics_device&, const mesh&);
        renderer(const renderer&) = delete;
        renderer& operator=(const renderer&) = delete;

        ~renderer();

        void draw(const camera&) const;

    private:
        SDL_GPUDevice* device_ {nullptr};
        SDL_Window* window_ {nullptr};
        gpu_buffer vertices_;
        gpu_buffer indices_;
        std::uint32_t index_count_ {0};
        SDL_GPUGraphicsPipeline* pipeline_ {nullptr};
        SDL_GPUTexture* depth_ {nullptr};
    };
}

#endif //CINDER_CITY_RENDERER_HPP
