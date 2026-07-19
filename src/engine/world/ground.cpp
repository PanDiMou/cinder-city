// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/world/ground.hpp"

namespace cinder {
    // Constructeur : il délègue au helper build() pour remplir mesh_.
    // La syntaxe ": mesh_ {build(size_meters)}" initialise mesh_ directement.
    ground::ground(const float size_meters) : mesh_ {build(size_meters)} {}

    // Construit le carré du sol à partir de sa taille en mètres.
    mesh ground::build(float size_meters) {
        // On centre le carré sur l'origine : il va de -half à +half sur chaque axe.
        const float half {size_meters * 0.5f};

        mesh out;
        // Les 4 coins du carré, à la hauteur Y = 0 (le sol). Chaque sommet est une
        // position (x, y, z). Le double {{ }} : l'extérieur = le vertex, l'intérieur
        // = son champ position (un vec3).
        out.vertices = {
            {{-half, 0.0f, -half}},   // 0 : coin arrière-gauche
            {{ half, 0.0f, -half}},   // 1 : coin arrière-droit
            {{ half, 0.0f,  half}},   // 2 : coin avant-droit
            {{-half, 0.0f,  half}},   // 3 : coin avant-gauche
        };

        // Les indices disent QUELS sommets forment chaque triangle (le GPU ne
        // dessine que des triangles). Ici 2 triangles (0-2-1 et 0-3-2) forment le
        // carré. L'ordre choisi oriente la "face avant" vers le haut (+Y).
        out.indices = {0, 2, 1, 0, 3, 2};

        return out;
    }
}
