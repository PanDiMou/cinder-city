// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "application.hpp"
#include "engine/world/static_prop.hpp"
#include "engine/scene/transform.hpp"
#include "engine/scene/scene_loader.hpp"

#include <SDL3/SDL.h>

namespace cinder {
    application::application() {
        // The grid floor.
        world_.spawn<static_prop>(ground_mesh_, transform{}, glm::vec4{0.30f, 0.30f, 0.32f, 1.0f}, material_type::grid_floor);

        // The city itself lives in data, not code: load the placed props from the scene file.
        load_scene("assets/city.json", world_, catalog_);

        SDL_SetWindowRelativeMouseMode(window_.native(), true); // capture the mouse for free-fly aiming
    }

    void application::run() {
        Uint64 last {SDL_GetTicksNS()};
        while (running_) {
            const Uint64 now {SDL_GetTicksNS()};
            const float delta {static_cast<float>(now - last) / 1'000'000'000.0f};
            last = now;
            process_events();
            world_.update(delta);
            update_camera(delta);
            render();
        }
    }

    void application::process_events() {
        constexpr float look_sensitivity {0.005f}; // radians per pixel

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running_ = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running_ = false;
                    }
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    camera_.look(-event.motion.xrel * look_sensitivity,
                                 -event.motion.yrel * look_sensitivity);
                    break;
                default:
                    break;
            }
        }
    }

    void application::update_camera(const float delta_seconds) {
        const bool* keys {SDL_GetKeyboardState(nullptr)};

        // Direction in camera-local space: x = right, y = up, z = forward.
        // Scancodes are physical: W/A/S/D sit where Z/Q/S/D are on an AZERTY keyboard.
        glm::vec3 direction {0.0f};
        if (keys[SDL_SCANCODE_W]) { direction.z += 1.0f; }
        if (keys[SDL_SCANCODE_S]) { direction.z -= 1.0f; }
        if (keys[SDL_SCANCODE_D]) { direction.x += 1.0f; }
        if (keys[SDL_SCANCODE_A]) { direction.x -= 1.0f; }
        if (keys[SDL_SCANCODE_SPACE])  { direction.y += 1.0f; }
        if (keys[SDL_SCANCODE_LSHIFT]) { direction.y -= 1.0f; }

        if (direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f) {
            camera_.move(glm::normalize(direction), delta_seconds);
        }
    }

    void application::render() const {
        renderer_.draw(camera_, world_);
    }
}
