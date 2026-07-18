// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_CAMERA_HPP
#define CINDER_CITY_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace cinder {
    // Free-fly camera: the mouse aims, the keyboard flies. Used to navigate the scene.
    class camera {
    public:
        // Aim: adjust orientation from relative mouse motion (radians).
        void look(const float delta_yaw, const float delta_pitch) {
            yaw_ += delta_yaw;
            pitch_ = glm::clamp(pitch_ + delta_pitch, min_pitch_, max_pitch_);
        }

        // Fly: move in camera-local space (x = right, y = world up, z = forward), in meters.
        void move(const glm::vec3& local, const float delta_seconds) {
            const glm::vec3 forward {direction()};
            const glm::vec3 right {glm::normalize(glm::cross(forward, up_))};
            position_ += (right * local.x + up_ * local.y + forward * local.z) * speed_ * delta_seconds;
        }

        [[nodiscard]] glm::mat4 view_projection(const float aspect) const {
            const glm::mat4 view {glm::lookAt(position_, position_ + direction(), up_)};
            const glm::mat4 projection {glm::perspective(fov_, aspect, near_, far_)};
            return projection * view;
        }

    private:
        // Unit forward vector from the yaw/pitch angles.
        [[nodiscard]] glm::vec3 direction() const {
            return {std::cos(pitch_) * std::sin(yaw_),
                    std::sin(pitch_),
                    std::cos(pitch_) * std::cos(yaw_)};
        }

        glm::vec3 position_ {0.0f, 12.0f, -30.0f};  // starting viewpoint
        glm::vec3 up_       {0.0f, 1.0f, 0.0f};

        float yaw_   {0.0f};                  // horizontal angle (radians), 0 = looking +Z
        float pitch_ {glm::radians(-15.0f)};  // vertical angle (radians), negative = looking down

        float speed_ {25.0f};                 // flight speed (meters per second)

        float min_pitch_ {glm::radians(-89.0f)}; // avoid flipping at the poles
        float max_pitch_ {glm::radians(89.0f)};

        float fov_  {glm::radians(60.0f)};
        float near_ {1.0f};
        float far_  {5000.0f};
    };
}

#endif //CINDER_CITY_CAMERA_HPP
