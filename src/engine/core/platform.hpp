// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// platform — initialise et éteint SDL (la bibliothèque bas niveau).
//
// SDL doit être "démarré" avant de créer une fenêtre, et "arrêté" à la fin.
// Cette classe utilise le patron RAII : démarrer SDL dans le constructeur,
// l'arrêter dans le destructeur. Ainsi, tant qu'un objet `platform` existe,
// SDL est prêt ; dès qu'il est détruit, SDL est nettoyé automatiquement.
//
// RAII = "Resource Acquisition Is Initialization" : une ressource est liée à
// la durée de vie d'un objet, donc on ne risque pas d'oublier de la libérer.
// ============================================================================

#ifndef CINDER_CITY_PLATFORM_HPP
#define CINDER_CITY_PLATFORM_HPP

namespace cinder {
    class platform {
    public:
        platform();    // constructeur : démarre SDL

        // Une seule instance doit exister. On INTERDIT donc la copie :
        // "= delete" supprime ces fonctions -> le compilateur refusera toute
        // tentative de copier un `platform`. Ça évite d'initialiser ou d'éteindre
        // SDL deux fois par erreur.
        platform(const platform&) = delete;
        platform& operator=(const platform&) = delete;

        ~platform();   // destructeur : arrête SDL (appelé tout seul en fin de vie de l'objet)
    };
}

#endif //CINDER_CITY_PLATFORM_HPP
