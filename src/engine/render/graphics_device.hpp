// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_GRAPHICS_DEVICE_HPP
#define CINDER_CITY_GRAPHICS_DEVICE_HPP

struct SDL_GPUDevice;
struct SDL_Window;

namespace cinder {
    class window;

    class graphics_device {
    public:
        explicit graphics_device(window&);
        graphics_device(const graphics_device&) = delete;
        graphics_device& operator=(const graphics_device&) = delete;

        ~graphics_device();

        [[nodiscard]] SDL_GPUDevice* native() const noexcept {
            return device_;
        }

        [[nodiscard]] SDL_Window* window_handle() const noexcept {
            return window_;
        }

    private:
        SDL_GPUDevice* device_ {nullptr};
        SDL_Window* window_ {nullptr};
    };
}

#endif //CINDER_CITY_GRAPHICS_DEVICE_HPP
