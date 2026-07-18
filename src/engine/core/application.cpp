// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "application.hpp"
#include "engine/world/static_prop.hpp"
#include "engine/world/player.hpp"
#include "engine/scene/transform.hpp"

#include <SDL3/SDL.h>

namespace cinder {
    application::application() {
        // The ground (green) and a 1 m orange cube resting on it at the center.
        world_.spawn<static_prop>(ground_mesh_, transform{}, glm::vec4{0.30f, 0.30f, 0.32f, 1.0f}, material_type::grid_floor);
        world_.spawn<static_prop>(building_mesh_, transform{.position = {10.0f, 0.0f, 0.0f}}, glm::vec4{0.72f, 0.72f, 0.75f, 1.0f});
        player_ = &world_.spawn<player>(cube_mesh_, transform{.position = {0.0f, 0.5f, 0.0f}}, glm::vec4{1.0f, 0.5f, 0.0f, 1.0f});
    }

    void application::run() {
        Uint64 last {SDL_GetTicksNS()};
        while (running_) {
            const Uint64 now {SDL_GetTicksNS()};
            const float delta {static_cast<float>(now - last) / 1'000'000'000.0f};
            last = now;
            process_events();
            world_.update(delta);
            camera_.follow(player_->get_transform().position);
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
