// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/scene/scene_loader.hpp"
#include "engine/scene/transform.hpp"
#include "engine/world/world.hpp"
#include "engine/world/static_prop.hpp"
#include "engine/world/entity.hpp"
#include "engine/assets/model_catalog.hpp"

#include <glm/gtc/quaternion.hpp>
#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>

namespace cinder {
    void load_scene(const std::string& path, world& world, model_catalog& catalog) {
        std::ifstream file {path};
        if (!file) {
            throw std::runtime_error("load_scene: cannot open " + path);
        }

        nlohmann::json data;
        try {
            file >> data;
        } catch (const nlohmann::json::parse_error& error) {
            throw std::runtime_error("load_scene(" + path + "): " + error.what());
        }

        // Each instance places one prop: which model, and where.
        for (const auto& instance : data.at("instances")) {
            const auto model {instance.at("model").get<std::string>()};
            const auto& position {instance.at("position")};

            transform transform;
            transform.position = {position.at(0).get<float>(),
                                  position.at(1).get<float>(),
                                  position.at(2).get<float>()};

            // Optional fields: default to no rotation and unit scale.
            const auto rotation_y {instance.value("rotation_y", 0.0f)};
            transform.rotation = glm::angleAxis(glm::radians(rotation_y), glm::vec3{0.0f, 1.0f, 0.0f});

            const auto scale {instance.value("scale", 1.0f)};
            transform.scale = glm::vec3{scale};

            world.spawn<static_prop>(catalog.get(model), transform, glm::vec4{1.0f}, material_type::textured);
        }
    }
}
