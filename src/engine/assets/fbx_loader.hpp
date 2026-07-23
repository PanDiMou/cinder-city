// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire, Consultation autorisée.
// Utilisation, copie, modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#ifndef CINDER_CITY_FBX_LOADER_HPP
#define CINDER_CITY_FBX_LOADER_HPP

#include <string>
#include "engine/render/mesh.hpp"

namespace cinder {
    // fbx_loader — charge un fichier .fbx (via ufbx) et le convertit en `mesh`
    // (sommets + indices + nom de texture), prête pour le moteur.
    [[nodiscard]] mesh load_fbx(const std::string& path);
}

#endif
