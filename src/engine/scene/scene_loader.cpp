// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "engine/scene/scene_loader.hpp"

#include <nlohmann/json.hpp>   // la bibliothèque qui lit/écrit le JSON

#include <fstream>    // std::ifstream (lire) / std::ofstream (écrire)
#include <stdexcept>

namespace cinder {
    std::vector<scene_instance> load_scene(const std::string& path) {
        // Ouvre le fichier en lecture. "!file" est vrai si l'ouverture a échoué.
        std::ifstream file {path};
        if (!file) {
            throw std::runtime_error("load_scene : impossible d'ouvrir " + path);
        }

        // Analyse (parse) le JSON. On protège avec try/catch car le fichier peut être mal formé.
        nlohmann::json data;
        try {
            file >> data;
        } catch (const nlohmann::json::parse_error& error) {
            throw std::runtime_error("load_scene(" + path + ") : " + error.what());
        }

        std::vector<scene_instance> instances;

        // On parcourt le tableau "instances" et on remplit notre liste.
        for (const auto& node : data.at("instances")) {
            scene_instance instance;
            instance.model = node.at("model").get<std::string>();

            const auto& position {node.at("position")};   // le tableau [x, y, z]
            instance.position = {position.at(0).get<float>(),
                                 position.at(1).get<float>(),
                                 position.at(2).get<float>()};

            // .value("clef", défaut) : lit un champ optionnel (défaut si absent).
            instance.rotation_y = node.value("rotation_y", 0.0f);
            instance.scale = node.value("scale", 1.0f);
            instance.variant = node.value("variant", std::string{});   // optionnel

            instances.push_back(instance);
        }

        return instances;
    }

    void save_scene(const std::string& path, const std::vector<scene_instance>& instances) {
        // On construit l'objet JSON à partir de la liste.
        nlohmann::json data;
        data["instances"] = nlohmann::json::array();   // un tableau vide au départ

        for (const scene_instance& instance : instances) {
            // On ajoute un objet JSON par instance. La syntaxe {{"clef", valeur}, ...}
            // décrit une paire clef/valeur.
            nlohmann::json entry {
                {"model", instance.model},
                {"position", {instance.position.x, instance.position.y, instance.position.z}},
                {"rotation_y", instance.rotation_y},
                {"scale", instance.scale}
            };
            // On n'écrit "variant" que s'il est renseigné, pour ne pas alourdir les
            // instances qui gardent leur apparence par défaut (la plupart).
            if (!instance.variant.empty()) {
                entry["variant"] = instance.variant;
            }
            data["instances"].push_back(entry);
        }

        // Ouvre le fichier en écriture (le crée/écrase). Échec -> erreur.
        std::ofstream file {path};
        if (!file) {
            throw std::runtime_error("save_scene : impossible d'écrire " + path);
        }
        // dump(2) : convertit le JSON en texte "joli" (indenté de 2 espaces).
        file << data.dump(2);
    }
}
