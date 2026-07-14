// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.
#ifndef CINDER_CITY_MESH_HPP
#define CINDER_CITY_MESH_HPP

#include <vector>

#include <glm/glm.hpp>

namespace cinder {
    struct vertex {
        glm::vec3 position;
    };

    struct mesh {
        std::vector<vertex> vertices;
        std::vector<std::uint32_t> indices;
    };
}

#endif //CINDER_CITY_MESH_HPP
