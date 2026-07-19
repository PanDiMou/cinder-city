// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "platform.hpp"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <string>

namespace cinder {
    // Constructeur : on démarre SDL avec le sous-système vidéo (fenêtre + affichage).
    platform::platform() {
        // SDL_Init renvoie "faux" en cas d'échec. Si ça échoue, on ne peut rien
        // faire d'utile, donc on lève une exception (throw) : elle remontera
        // jusqu'au try/catch de main().
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            // SDL_GetError() donne la raison précise de l'échec, côté SDL.
            throw std::runtime_error(std::string{"Échec de SDL_Init : "} + SDL_GetError());
        }
    }

    // Destructeur : appelé automatiquement quand l'objet `platform` disparaît.
    // On éteint proprement SDL. C'est la moitié "libération" du RAII.
    platform::~platform() {
        SDL_Quit();
    }
}
