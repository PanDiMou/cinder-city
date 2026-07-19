// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// scene_loader — lit et écrit la "scène" (la liste des bâtiments) en JSON.
//
// La scène est décrite comme une simple liste de données : chaque `scene_instance`
// dit QUEL modèle, OÙ, avec quelle rotation/échelle. C'est la SOURCE DE VÉRITÉ :
//   - load_scene() la lit depuis le fichier,
//   - l'application la transforme en entités visibles (spawn),
//   - save_scene() la réécrit sur le disque.
//
// Ce découpage permet de sauvegarder : on garde la liste en mémoire, on la modifie
// (poser un bâtiment = ajouter une instance), puis on la réécrit.
// ============================================================================

#ifndef CINDER_CITY_SCENE_LOADER_HPP
#define CINDER_CITY_SCENE_LOADER_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace cinder {
    // Une "instance" = un bâtiment placé dans la scène (pure donnée, pas de rendu).
    struct scene_instance {
        std::string model;                 // nom du modèle (ex : "SM_Bld_Beach_Shop_01")
        glm::vec3   position {0.0f};       // où le poser
        float       rotation_y {0.0f};     // rotation autour de l'axe vertical, en degrés
        float       scale {1.0f};          // échelle uniforme
    };

    // Lit un fichier de scène JSON et renvoie la liste des instances.
    // Lève std::runtime_error en cas d'échec (fichier absent, JSON invalide).
    [[nodiscard]] std::vector<scene_instance> load_scene(const std::string& path);

    // Écrit la liste des instances dans un fichier de scène JSON.
    // Lève std::runtime_error si l'écriture échoue.
    void save_scene(const std::string& path, const std::vector<scene_instance>& instances);
}

#endif //CINDER_CITY_SCENE_LOADER_HPP
