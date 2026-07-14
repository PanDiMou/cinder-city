// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_GROUND_HPP
#define CINDER_CITY_GROUND_HPP

#include "engine/render/mesh.hpp"

namespace cinder {
    // Flat, square, horizontal ground (X/Z plane, Y up), centered at the origin.
    // Unit: 1 unit = 1 meter. Defaults to 1 km x 1 km.
    class ground {
    public:
        explicit ground(float = 1000.0f);

        [[nodiscard]] const mesh& geometry() const noexcept { return mesh_; }

    private:
        static mesh build(float);

        mesh mesh_;
    };
}

#endif //CINDER_CITY_GROUND_HPP
