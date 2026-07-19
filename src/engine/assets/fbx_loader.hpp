// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// fbx_loader — importe un modèle 3D depuis un fichier .fbx.
//
// Le FBX est un format d'échange 3D (celui de tes assets Synty). On ne le lit
// pas nous-mêmes (format complexe) : on s'appuie sur la bibliothèque "ufbx".
// Cette fonction transforme un fichier .fbx en une `mesh` (sommets + indices)
// utilisable par le moteur.
//
// Ce fichier n'expose qu'UNE fonction : load_fbx.
// ============================================================================

#ifndef CINDER_CITY_FBX_LOADER_HPP
#define CINDER_CITY_FBX_LOADER_HPP

#include <string>

#include "engine/render/mesh.hpp"   // le type `mesh` qu'on renvoie

namespace cinder {
    // Charge un mesh statique depuis un fichier FBX, converti en mètres et Y-up
    // (nos conventions). Toutes les pièces du fichier sont fusionnées en une seule
    // géométrie. Lève std::runtime_error si le chargement échoue.
    //  - [[nodiscard]] : n'ignore pas le mesh renvoyé (sinon le chargement serait inutile).
    [[nodiscard]] mesh load_fbx(const std::string& path);
}

#endif //CINDER_CITY_FBX_LOADER_HPP
