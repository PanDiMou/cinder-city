// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/world/cube.hpp"

namespace cinder {
    cube::cube(const float size_meters) : mesh_ {build(size_meters)} {}

    mesh cube::build(const float size_meters) {
        const float h {size_meters * 0.5f};

        mesh out;
        out.vertices = {
            {{-h, -h, -h}}, {{ h, -h, -h}}, {{ h,  h, -h}}, {{-h,  h, -h}}, // z = -h
            {{-h, -h,  h}}, {{ h, -h,  h}}, {{ h,  h,  h}}, {{-h,  h,  h}}, // z = +h
        };
        // 12 triangles (2 per face), outward-facing.
        out.indices = {
            4, 5, 6,  6, 7, 4,   // +Z
            0, 3, 2,  2, 1, 0,   // -Z
            1, 2, 6,  6, 5, 1,   // +X
            0, 4, 7,  7, 3, 0,   // -X
            3, 7, 6,  6, 2, 3,   // +Y
            0, 1, 5,  5, 4, 0,   // -Y
        };

        return out;
    }
}
