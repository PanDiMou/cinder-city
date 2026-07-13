// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "application.hpp"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <string>

namespace cinder {
    application::application() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::string{"SDL_Init: "} + SDL_GetError());
        }

        window_.emplace();
    }

    application::~application() {
        window_.reset();
        SDL_Quit();
    }

    void application::run() {
        while (running_) {
            process_events();
        }
    }

    void application::process_events() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running_ = false;
            }
        }
    }
}
