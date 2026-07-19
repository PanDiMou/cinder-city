// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_RENDERER_HPP
#define CINDER_CITY_RENDERER_HPP

#include "engine/render/texture.hpp"

struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUTexture;

namespace cinder {
    class graphics_device;
    class camera;
    class world;
    class ui;

    // Owns the graphics pipeline and draws the world each frame.
    class renderer {
    public:
        explicit renderer(const graphics_device&);
        renderer(const renderer&) = delete;
        renderer& operator=(const renderer&) = delete;

        ~renderer();

        void draw(const camera&, const world&, ui&) const;

    private:
        SDL_GPUDevice* device_ {nullptr};
        SDL_Window* window_ {nullptr};
        SDL_GPUGraphicsPipeline* solid_color_pipeline_ {nullptr}; // solid color material
        SDL_GPUGraphicsPipeline* grid_floor_pipeline_ {nullptr};  // grid floor material
        SDL_GPUGraphicsPipeline* textured_pipeline_ {nullptr};    // palette-textured material
        SDL_GPUTexture* depth_ {nullptr};
        texture palette_; // shared palette texture for imported models (one for now)
    };
}

#endif //CINDER_CITY_RENDERER_HPP
