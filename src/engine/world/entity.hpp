// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_ENTITY_HPP
#define CINDER_CITY_ENTITY_HPP

#include "engine/scene/transform.hpp"

namespace cinder {
    class gpu_mesh;

    // Which shader/pipeline draws the entity.
    enum class material_type {
        solid_color, // solid color, no lighting
        grid_floor,  // gray floor with a procedural grid
        textured     // sampled from the palette texture (imported models)
    };

    // Base class for everything living in the world: a transform + a shared mesh to draw.
    class entity {
    public:
        entity(const gpu_mesh &mesh, const transform &transform, const glm::vec4 &color, const material_type material = material_type::solid_color)
            : transform_{transform}, mesh_{&mesh}, color_{color}, material_{material} { }
        entity(const entity&) = delete;
        entity& operator=(const entity&) = delete;

        virtual ~entity() = default;

        // Called every frame. Default: do nothing (static objects never move).
        virtual void update(float /*delta_seconds*/) { }

        [[nodiscard]] const transform &get_transform() const noexcept { return transform_; }
        [[nodiscard]] const gpu_mesh &mesh() const noexcept { return *mesh_; }
        [[nodiscard]] const glm::vec4 &color() const noexcept { return color_; }
        [[nodiscard]] material_type material() const noexcept { return material_; }
    protected:
        transform transform_;
    private:
        const gpu_mesh* mesh_;
        glm::vec4 color_;
        material_type material_;
    };
}

#endif //CINDER_CITY_ENTITY_HPP
