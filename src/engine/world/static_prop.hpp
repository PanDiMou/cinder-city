// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_STATIC_PROP_HPP
#define CINDER_CITY_STATIC_PROP_HPP

#include "engine/world/entity.hpp"

namespace cinder {
    class static_prop final: public entity {
    public:
        using entity::entity;
        // no update() override -> keeps the default no-op -> never moves
    };
}

#endif //CINDER_CITY_STATIC_PROP_HPP
