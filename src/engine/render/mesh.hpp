// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

// ============================================================================
// mesh — la géométrie brute d'un objet, côté CPU (dans la mémoire normale).
//
// Un objet 3D est fait de sommets (des points) reliés en triangles. Ici :
//   - `vertex`  = un point, avec sa position et sa coordonnée de texture.
//   - `mesh`    = la liste des sommets + la liste des indices (quels sommets
//                 forment quels triangles).
//
// C'est la forme "en RAM". Pour l'afficher, on l'envoie ensuite au GPU sous
// forme de `gpu_mesh`.
// ============================================================================

#ifndef CINDER_CITY_MESH_HPP
#define CINDER_CITY_MESH_HPP

#include <vector>       // std::vector = tableau de taille variable
#include <glm/glm.hpp>  // vec3, vec2

namespace cinder {
    // Un sommet : un point de la surface 3D.
    struct vertex {
        glm::vec3 position;  // où il se trouve dans l'espace (x, y, z)
        glm::vec2 uv;        // où aller chercher sa couleur dans la texture (u, v)
    };

    // Un mesh : l'ensemble de la géométrie.
    struct mesh {
        std::vector<vertex> vertices;          // la liste des sommets
        // Les indices référencent des sommets par leur position dans `vertices`.
        // Trois indices = un triangle. Ça évite de répéter les sommets partagés.
        std::vector<std::uint32_t> indices;
    };
}

#endif //CINDER_CITY_MESH_HPP
