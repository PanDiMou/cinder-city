// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_TRANSFORM_HPP
#define CINDER_CITY_TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cinder {
    struct transform {
        glm::vec3 position {0.0f};
        glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 scale    {1.0f};

        [[nodiscard]] glm::mat4 matrix() const {
            const glm::mat4 translation {glm::translate(glm::mat4{1.0f}, position)};
            const glm::mat4 orientation {glm::mat4_cast(rotation)};
            const glm::mat4 scaling     {glm::scale(glm::mat4{1.0f}, scale)};

            return translation * orientation * scaling;
        }
    };
}

#endif //CINDER_CITY_TRANSFORM_HPP
