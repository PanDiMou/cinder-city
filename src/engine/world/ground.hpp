// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// ground — fabrique la géométrie du SOL (un grand carré plat).
//
// Contrairement aux bâtiments (importés depuis des fichiers FBX), le sol est
// généré "à la main" par du code : 4 coins, 2 triangles. Cette classe ne fait
// que produire cette géométrie ; c'est ensuite un gpu_mesh qui l'envoie au GPU,
// et un static_prop qui la place dans le monde.
// ============================================================================

#ifndef CINDER_CITY_GROUND_HPP
#define CINDER_CITY_GROUND_HPP

#include "engine/render/mesh.hpp"

namespace cinder {
    // Sol plat, carré, horizontal (plan X/Z, Y vers le haut), centré à l'origine.
    // Unité : 1 unité = 1 mètre. Par défaut 1 km x 1 km.
    class ground {
    public:
        // "explicit" empêche une conversion automatique surprenante (ex : passer
        // un float là où un ground est attendu). Le "= 1000.0f" est la taille par
        // défaut si on ne précise rien : ground g; donne un sol de 1000 m.
        explicit ground(float = 1000.0f);

        // Renvoie la géométrie construite (les sommets + indices), en lecture seule.
        [[nodiscard]] const mesh& geometry() const noexcept { return mesh_; }

    private:
        // "static" = fonction utilitaire qui n'a pas besoin d'un objet ground pour
        // tourner ; elle sert juste à calculer la géométrie dans le constructeur.
        static mesh build(float);

        mesh mesh_;   // la géométrie (positions des sommets + ordre des triangles)
    };
}

#endif //CINDER_CITY_GROUND_HPP
