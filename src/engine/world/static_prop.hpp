// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// static_prop — un objet du décor qui ne bouge JAMAIS (sol, bâtiment, arbre...).
//
// C'est le cas le plus simple d'entité : il n'ajoute aucun comportement, il
// hérite juste de tout ce que fait `entity`. C'est un bon exemple d'héritage
// minimal en C++.
// ============================================================================

#ifndef CINDER_CITY_STATIC_PROP_HPP
#define CINDER_CITY_STATIC_PROP_HPP

#include "engine/world/entity.hpp"

namespace cinder {
    // ": public entity" = static_prop HÉRITE d'entity (il "est un" entity).
    // "final" = on interdit d'hériter encore de static_prop (pas de classe fille).
    class static_prop final: public entity {
    public:
        // "using entity::entity;" = on réutilise tels quels les constructeurs de la
        // classe de base. Sans ça, il faudrait réécrire un constructeur qui ne fait
        // que transmettre les arguments à entity. Ici, c'est automatique.
        using entity::entity;

        // On ne redéfinit PAS update() : static_prop garde donc la version par
        // défaut d'entity (qui ne fait rien) -> l'objet reste immobile.
    };
}

#endif //CINDER_CITY_STATIC_PROP_HPP
