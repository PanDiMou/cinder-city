// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "application.hpp"

#include <SDL3/SDL.h>

namespace cinder {
    void application::run() {
        while (running_) {
            process_events();
            render();
        }
    }

    void application::process_events() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT
                || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
                running_ = false;
            }
        }
    }

    void application::render() const {
        renderer_.draw(camera_);
    }
}
