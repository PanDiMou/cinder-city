// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/world/player.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace cinder {
    namespace {
        constexpr float speed {5.0f}; // meters per second
    }

    void player::update(const float delta_seconds) {
        const bool* keys {SDL_GetKeyboardState(nullptr)};

        glm::vec3 direction {0.0f};
        if (keys[SDL_SCANCODE_UP]) {
            direction.z -= 1.0f;
        }

        if (keys[SDL_SCANCODE_DOWN]) {
            direction.z += 1.0f;
        }

        if (keys[SDL_SCANCODE_LEFT]) {
            direction.x -= 1.0f;
        }

        if (keys[SDL_SCANCODE_RIGHT]) {
            direction.x += 1.0f;
        }

        if (direction.x != 0.0f || direction.z != 0.0f) {
            transform_.position += glm::normalize(direction) * speed * delta_seconds;
        }
    }
}
