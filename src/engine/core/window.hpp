// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

// ============================================================================
// window — la fenêtre du jeu (plein écran).
//
// Enveloppe RAII autour d'un SDL_Window : la fenêtre est créée dans le
// constructeur et détruite dans le destructeur. Les autres classes (le GPU, la
// souris...) ont besoin de la fenêtre : elles y accèdent via native().
// ============================================================================

#ifndef CINDER_CITY_WINDOW_HPP
#define CINDER_CITY_WINDOW_HPP

// "Déclaration anticipée" (forward declaration) : on annonce juste que le type
// SDL_Window existe, sans inclure tout SDL ici. Ça garde le header léger. Le
// vrai include de SDL est dans window.cpp, où on a besoin des détails.
struct SDL_Window;

namespace cinder {
    class window {
    public:
        window();    // crée la fenêtre
        window(const window&) = delete;              // pas de copie (une seule fenêtre)
        window& operator=(const window&) = delete;

        ~window();   // détruit la fenêtre

        // native() donne accès au SDL_Window brut, pour les fonctions SDL qui
        // en ont besoin. Décryptage des mots-clés :
        //  - [[nodiscard]] : le compilateur avertit si on ignore la valeur renvoyée.
        //  - const  : cette méthode ne modifie pas la fenêtre.
        //  - noexcept : cette méthode ne lève jamais d'exception (elle ne fait
        //               que renvoyer un pointeur déjà là).
        [[nodiscard]] SDL_Window* native() const noexcept {
            return handle_;
        }
    private:
        // Le pointeur vers la fenêtre SDL. "{nullptr}" = initialisé à "rien"
        // au départ (bonne habitude : jamais de pointeur non initialisé).
        SDL_Window* handle_ {nullptr};
    };
}

#endif //CINDER_CITY_WINDOW_HPP
