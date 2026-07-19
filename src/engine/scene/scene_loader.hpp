// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// scene_loader — charge une "scène" (la liste des bâtiments à poser) depuis un
// fichier de données JSON (assets/city.json).
//
// Idée importante : la disposition de la ville est dans un FICHIER, pas dans le
// code. On peut donc ajouter/déplacer des bâtiments en éditant le fichier, sans
// recompiler. Cette classe fait le pont : fichier JSON -> entités dans le monde.
//
// Ce fichier ne déclare qu'UNE fonction (pas de classe) : load_scene.
// ============================================================================

#ifndef CINDER_CITY_SCENE_LOADER_HPP
#define CINDER_CITY_SCENE_LOADER_HPP

#include <string>

namespace cinder {
    // Déclarations anticipées : on n'a besoin que de références ici, pas des détails.
    class world;
    class model_catalog;

    // Lit un fichier de scène JSON et crée ses props dans le monde, en résolvant
    // chaque nom de modèle via le catalogue. Lève std::runtime_error en cas d'échec.
    //  - path    : chemin du fichier (ex : "assets/city.json")
    //  - world   : le monde où ajouter les entités (référence -> il sera modifié)
    //  - catalog : sert à obtenir la géométrie de chaque modèle
    void load_scene(const std::string&, world&, model_catalog&);
}

#endif //CINDER_CITY_SCENE_LOADER_HPP
