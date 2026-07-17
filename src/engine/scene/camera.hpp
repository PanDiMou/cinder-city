// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_CAMERA_HPP
#define CINDER_CITY_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cinder {
    // Perspective camera. Produces the view-projection matrix sent to the shader.
    class camera {
    public:
        [[nodiscard]] glm::mat4 view_projection(const float aspect) const {
            const glm::mat4 view {glm::lookAt(eye_, target_, up_)};
            glm::mat4 projection {glm::perspective(fov_, aspect, near_, far_)};
            return projection * view;
        }

    private:
        glm::vec3 eye_    {0.0f, 3.0f, 6.0f};      // close, to see the 1 m cube
        glm::vec3 target_ {0.0f, 0.5f, 0.0f};      // looking at the cube's center
        glm::vec3 up_     {0.0f, 1.0f, 0.0f};

        float fov_  {glm::radians(60.0f)};
        float near_ {1.0f};
        float far_  {5000.0f};
    };
}

#endif //CINDER_CITY_CAMERA_HPP
