// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/assets/fbx_loader.hpp"

#include <ufbx.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace cinder {
    mesh load_fbx(const std::string& path) {
        // Bring the file into our conventions: meters, Y-up, right-handed.
        ufbx_load_opts opts {};
        opts.target_axes = ufbx_axes_right_handed_y_up;
        opts.target_unit_meters = 1.0f;
        opts.space_conversion = UFBX_SPACE_CONVERSION_TRANSFORM_ROOT;

        ufbx_error error;
        ufbx_scene* scene {ufbx_load_file(path.c_str(), &opts, &error)};
        if (scene == nullptr) {
            throw std::runtime_error("load_fbx(" + path + "): " + error.description.data);
        }

        mesh out;

        // Merge every mesh node into one geometry, baking each node's world
        // transform into the vertex positions (static prop: no per-part motion).
        for (std::size_t node_index {0}; node_index < scene->nodes.count; ++node_index) {
            const ufbx_node* node {scene->nodes.data[node_index]};
            if (node->is_root || node->mesh == nullptr) {
                continue;
            }

            const ufbx_mesh* part {node->mesh};

            // Scratch buffer big enough for the largest polygon in this part.
            std::vector<std::uint32_t> triangle_indices(part->max_face_triangles * 3);

            for (std::size_t face_index {0}; face_index < part->faces.count; ++face_index) {
                const ufbx_face face {part->faces.data[face_index]};

                // Split the (possibly n-gon) face into triangles.
                const std::uint32_t triangles {
                    ufbx_triangulate_face(triangle_indices.data(), triangle_indices.size(), part, face)
                };

                for (std::size_t i {0}; i < static_cast<std::size_t>(triangles) * 3; ++i) {
                    const std::uint32_t index {triangle_indices[i]};
                    const ufbx_vec3 local {ufbx_get_vertex_vec3(&part->vertex_position, index)};
                    const ufbx_vec3 world {ufbx_transform_position(&node->geometry_to_world, local)};

                    out.indices.push_back(static_cast<std::uint32_t>(out.vertices.size()));
                    out.vertices.push_back(vertex {
                        .position = {static_cast<float>(world.x),
                                     static_cast<float>(world.y),
                                     static_cast<float>(world.z)}
                    });
                }
            }
        }

        ufbx_free_scene(scene);

        if (out.vertices.empty()) {
            throw std::runtime_error("load_fbx(" + path + "): no mesh geometry found");
        }
        return out;
    }
}
