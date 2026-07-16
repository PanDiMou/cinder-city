// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_RENDERER_HPP
#define CINDER_CITY_RENDERER_HPP

struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUTexture;

namespace cinder {
    class graphics_device;
    class camera;
    class world;

    // Owns the graphics pipeline and draws the world each frame.
    class renderer {
    public:
        explicit renderer(const graphics_device&);
        renderer(const renderer&) = delete;
        renderer& operator=(const renderer&) = delete;

        ~renderer();

        void draw(const camera&, const world&) const;

    private:
        SDL_GPUDevice* device_ {nullptr};
        SDL_Window* window_ {nullptr};
        SDL_GPUGraphicsPipeline* pipeline_ {nullptr};
        SDL_GPUTexture* depth_ {nullptr};
    };
}

#endif //CINDER_CITY_RENDERER_HPP
