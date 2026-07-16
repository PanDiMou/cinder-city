// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "application.hpp"
#include "engine/world/static_prop.hpp"
#include "engine/scene/transform.hpp"

#include <SDL3/SDL.h>

namespace cinder {
    application::application() {
        // The ground becomes the first entity of the world, placed at the origin.
        world_.spawn<static_prop>(ground_mesh_, transform {});
    }

    void application::run() {
        Uint64 last {SDL_GetTicksNS()};
        while (running_) {
            const Uint64 now {SDL_GetTicksNS()};
            const float delta {static_cast<float>(now - last) / 1'000'000'000.0f};
            last = now;
            process_events();
            world_.update(delta);
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
        renderer_.draw(camera_, world_);
    }
}
