// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// transform — OÙ et COMMENT un objet est placé dans le monde 3D.
//
// Trois informations : position (où), rotation (dans quel sens), scale (quelle
// taille). La méthode matrix() combine ces trois infos en UNE matrice 4x4 que
// le GPU sait appliquer aux sommets pour placer l'objet.
//
// C'est un "struct" (comme une classe, mais tout est public par défaut) car ce
// n'est qu'un simple sac de données.
// ============================================================================

#ifndef CINDER_CITY_TRANSFORM_HPP
#define CINDER_CITY_TRANSFORM_HPP

#include <glm/glm.hpp>                    // GLM = la bibliothèque de maths 3D
#include <glm/gtc/matrix_transform.hpp>   // translate, scale
#include <glm/gtc/quaternion.hpp>         // quaternions (rotations)

namespace cinder {
    struct transform {
        // vec3 = 3 nombres (x, y, z). "{0.0f}" met tout à 0 -> à l'origine.
        glm::vec3 position {0.0f};

        // quat = quaternion : une façon robuste de représenter une rotation (mieux
        // que 3 angles, car pas de "blocage de cardan"). {1,0,0,0} = aucune rotation.
        glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};

        // Échelle sur chaque axe. {1.0f} = taille normale (x1). 2.0f = deux fois plus grand.
        glm::vec3 scale    {1.0f};

        // Construit la matrice de transformation finale (position + rotation + taille).
        [[nodiscard]] glm::mat4 matrix() const {
            // On fabrique une matrice pour chaque étape. "mat4{1.0f}" = la matrice
            // identité (ne change rien), qu'on modifie ensuite.
            const glm::mat4 translation {glm::translate(glm::mat4{1.0f}, position)}; // déplacement
            const glm::mat4 orientation {glm::mat4_cast(rotation)};                  // rotation (quat -> matrice)
            const glm::mat4 scaling     {glm::scale(glm::mat4{1.0f}, scale)};        // mise à l'échelle

            // ATTENTION à l'ordre (les matrices ne sont pas commutatives). Lu de
            // DROITE à gauche : on met d'abord à l'échelle, puis on tourne, puis on
            // déplace. C'est l'ordre naturel : T * R * S.
            return translation * orientation * scaling;
        }
    };
}

#endif //CINDER_CITY_TRANSFORM_HPP
