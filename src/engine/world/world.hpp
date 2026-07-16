// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_WORLD_HPP
#define CINDER_CITY_WORLD_HPP

#include "engine/world/entity.hpp"

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

namespace cinder {
    // Owns every entity in the game and updates them each frame.
    class world {
    public:
        // Creates an entity of type T in the world and returns a reference to it.
        template <std::derived_from<entity> T, typename... Args>
        T& spawn(Args&&... args) {
            auto created = std::make_unique<T>(std::forward<Args>(args)...);
            T& reference {*created};
            entities_.push_back(std::move(created));
            return reference;
        }

        void update(const float delta_seconds) {
            for (const auto& entity : entities_) {
                entity->update(delta_seconds);
            }
        }

        [[nodiscard]] const std::vector<std::unique_ptr<entity>>& entities() const noexcept {
            return entities_;
        }

    private:
        std::vector<std::unique_ptr<entity>> entities_;
    };
}

#endif //CINDER_CITY_WORLD_HPP
