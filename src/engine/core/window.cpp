// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "window.hpp"

#include <stdexcept>
#include <SDL3/SDL.h>
#include <string>

namespace cinder {
    window::window() {
        constexpr int fallback_w {1280};
        constexpr int fallback_h {720};

        handle_ = SDL_CreateWindow("Cinder City", fallback_w, fallback_h, SDL_WINDOW_FULLSCREEN);
        if (handle_ == nullptr) {
            throw std::runtime_error(std::string{"SDL_CreateWindow: "} + SDL_GetError());
        }
    }

    window::~window() {
        if (handle_ != nullptr) {
            SDL_DestroyWindow(handle_);
        }
    }
}
