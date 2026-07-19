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

#include <glm/gtc/quaternion.hpp>   // glm::angleAxis (fabrique une rotation)
#include <nlohmann/json.hpp>        // la bibliothèque qui lit le JSON

#include <fstream>    // std::ifstream (lire un fichier)
#include <stdexcept>

namespace cinder {
    void load_scene(const std::string& path, world& world, model_catalog& catalog) {
        // Ouvre le fichier en lecture. Si l'ouverture échoue (fichier absent...),
        // le test "!file" est vrai, et on lève une erreur.
        std::ifstream file {path};
        if (!file) {
            throw std::runtime_error("load_scene : impossible d'ouvrir " + path);
        }

        // Lit et analyse (parse) le contenu JSON. On entoure d'un try/catch car le
        // JSON peut être mal formé (virgule oubliée, etc.) -> nlohmann lève alors une erreur.
        nlohmann::json data;
        try {
            file >> data;   // remplit `data` avec le contenu du fichier
        } catch (const nlohmann::json::parse_error& error) {
            throw std::runtime_error("load_scene(" + path + ") : " + error.what());
        }

        // On parcourt le tableau "instances" du JSON. Chaque élément = un bâtiment.
        for (const auto& instance : data.at("instances")) {
            // .at("model") lit le champ "model" ; .get<std::string>() le convertit en texte.
            const auto model {instance.at("model").get<std::string>()};
            const auto& position {instance.at("position")};   // le tableau [x, y, z]

            // On construit le transform (position/rotation/échelle) de ce bâtiment.
            transform transform;
            transform.position = {position.at(0).get<float>(),   // x
                                  position.at(1).get<float>(),   // y
                                  position.at(2).get<float>()};  // z

            // .value("clef", défaut) lit un champ OPTIONNEL : si absent, on prend le défaut.
            // Ici la rotation autour de l'axe vertical (Y), en degrés.
            const auto rotation_y {instance.value("rotation_y", 0.0f)};
            transform.rotation = glm::angleAxis(glm::radians(rotation_y), glm::vec3{0.0f, 1.0f, 0.0f});

            const auto scale {instance.value("scale", 1.0f)};   // échelle uniforme, 1 par défaut
            transform.scale = glm::vec3{scale};

            // Crée l'entité : on demande au catalogue la géométrie du modèle
            // (catalog.get(model)), et on la pose comme prop statique texturé.
            world.spawn<static_prop>(catalog.get(model), transform, glm::vec4{1.0f}, material_type::textured);
        }
    }
}
