// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "window.hpp"

#include <stdexcept>
#include <SDL3/SDL.h>
#include <string>

namespace cinder {
    window::window() {
        // "constexpr" = constante connue à la compilation. Ces valeurs ne servent
        // que de repli : en plein écran, SDL utilise la résolution de l'écran,
        // mais il faut quand même lui passer une taille.
        constexpr int fallback_w {1280};
        constexpr int fallback_h {720};

        // Crée la fenêtre. Le dernier argument SDL_WINDOW_FULLSCREEN = plein écran.
        handle_ = SDL_CreateWindow("Cinder City", fallback_w, fallback_h, SDL_WINDOW_FULLSCREEN);
        if (handle_ == nullptr) {
            // nullptr = "aucune fenêtre" -> la création a échoué, on lève une erreur.
            throw std::runtime_error(std::string{"Échec de SDL_CreateWindow : "} + SDL_GetError());
        }
    }

    window::~window() {
        // On ne détruit que si la fenêtre existe vraiment (sécurité).
        if (handle_ != nullptr) {
            SDL_DestroyWindow(handle_);
        }
    }
}
