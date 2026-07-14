// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "platform.hpp"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <string>

namespace cinder {
    platform::platform() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::string{"SDL_Init: "} + SDL_GetError());
        }
    }

    platform::~platform() {
        SDL_Quit();
    }
}
