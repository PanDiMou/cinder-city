// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_CUBE_HPP
#define CINDER_CITY_CUBE_HPP

#include "engine/render/mesh.hpp"

namespace cinder {
    // Cube centered at the origin. Unit: 1 unit = 1 meter. Defaults to 1 m³.
    class cube {
    public:
        explicit cube(float = 1.0f);

        [[nodiscard]] const mesh& geometry() const noexcept { return mesh_; }

    private:
        static mesh build(float);

        mesh mesh_;
    };
}

#endif //CINDER_CITY_CUBE_HPP
