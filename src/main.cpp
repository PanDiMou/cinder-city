// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// main.cpp — le point d'entrée du programme.
//
// C'est la toute première fonction exécutée quand on lance le jeu. Son rôle est
// minuscule : créer l'application, la lancer, et attraper les erreurs graves.
// Tout le vrai travail est délégué à la classe `application`.
// ============================================================================

#include "engine/core/application.hpp"
#include "engine/core/log.hpp"

#include <cstdlib>     // pour EXIT_SUCCESS / EXIT_FAILURE
#include <exception>   // pour std::exception (le type des erreurs qu'on attrape)

int main() {
    // try/catch : on "essaie" de lancer le jeu, et si une erreur grave est
    // levée (un `throw` quelque part dans le code), on la rattrape ici au lieu
    // de laisser le programme planter brutalement.
    try {
        cinder::log::info("Cinder City — démarrage");

        cinder::application app;   // construit l'application (fenêtre, GPU, scène...)
        app.run();                 // lance la boucle de jeu (tourne jusqu'à la fermeture)

        return EXIT_SUCCESS;       // 0 : tout s'est bien passé
    } catch (const std::exception& exception) {
        // Attrape toute erreur "standard" et affiche son message.
        // .what() renvoie le texte de l'erreur (ex : "Échec de SDL_Init : ...").
        cinder::log::error("Erreur fatale : {}", exception.what());
    } catch (...) {
        // Le "..." attrape absolument tout le reste (erreurs non standard).
        cinder::log::error("Erreur fatale inconnue");
    }

    return EXIT_FAILURE;   // valeur non nulle : le programme s'est terminé sur une erreur
}
