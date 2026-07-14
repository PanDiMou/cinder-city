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
        void set_aspect(const float aspect) noexcept { aspect_ = aspect; }

        [[nodiscard]] glm::mat4 view_projection() const {
            const glm::mat4 view {glm::lookAt(eye_, target_, up_)};
            glm::mat4 projection {glm::perspective(fov_, aspect_, near_, far_)};
            projection[1][1] *= -1.0f; // SDL_gpu clip space: flip Y
            return projection * view;
        }

    private:
        glm::vec3 eye_    {0.0f, 350.0f, 600.0f}; // high up and pulled back
        glm::vec3 target_ {0.0f, 0.0f, 0.0f};     // looking at the origin
        glm::vec3 up_     {0.0f, 1.0f, 0.0f};

        float aspect_ {16.0f / 9.0f};
        float fov_    {glm::radians(60.0f)};
        float near_   {1.0f};
        float far_    {5000.0f};
    };
}

#endif //CINDER_CITY_CAMERA_HPP
