// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/world/ground.hpp"

namespace cinder {
    ground::ground(const float size_meters) : mesh_ {build(size_meters)} {}

    mesh ground::build(float size_meters) {
        const float half {size_meters * 0.5f};

        mesh out;
        out.vertices = {
            {{-half, 0.0f, -half}},
            {{ half, 0.0f, -half}},
            {{ half, 0.0f,  half}},
            {{-half, 0.0f,  half}},
        };
        // Two triangles, front face pointing up (+Y).
        out.indices = {0, 2, 1, 0, 3, 2};

        return out;
    }
}
