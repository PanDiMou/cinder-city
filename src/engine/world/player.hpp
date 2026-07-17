// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_PLAYER_HPP
#define CINDER_CITY_PLAYER_HPP

#include "engine/world/entity.hpp"

namespace cinder {
    // An entity moved by the arrow keys on the X/Z (ground) plane.
    class player final : public entity {
    public:
        using entity::entity;

        void update(float) override;
    };
}

#endif //CINDER_CITY_PLAYER_HPP
