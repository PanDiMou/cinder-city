// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "application.hpp"
#include "engine/world/static_prop.hpp"
#include "engine/scene/transform.hpp"
#include "engine/scene/scene_loader.hpp"

#include <imgui.h>
#include <SDL3/SDL.h>

namespace cinder {
    // Constructeur : prépare la scène de départ.
    application::application() {
        // Ajoute le sol au monde. spawn<static_prop> crée une entité "prop statique"
        // (qui ne bouge jamais). Les arguments : la géométrie (ground_mesh_), sa
        // position/rotation (transform{} = par défaut, à l'origine), sa couleur, et
        // le matériau grid_floor (le shader qui dessine le quadrillage gris).
        world_.spawn<static_prop>(ground_mesh_, transform{}, glm::vec4{0.30f, 0.30f, 0.32f, 1.0f}, material_type::grid_floor);

        // La ville vit dans les données, pas dans le code : on charge les bâtiments
        // depuis le fichier de scène. Modifier city.json suffit à changer la ville.
        load_scene("assets/city.json", world_, catalog_);

        // On démarre en "mode vol" : la souris est capturée et pilote la caméra.
        // La touche Tab bascule ensuite vers le "mode curseur" (pour cliquer dans l'UI).
        set_fly_mode(true);
    }

    // run() = la BOUCLE DE JEU. Elle tourne tant que running_ est vrai.
    void application::run() {
        // On mémorise l'instant précédent pour mesurer le temps écoulé entre 2 frames.
        Uint64 last {SDL_GetTicksNS()}; // temps actuel en nanosecondes

        while (running_) {
            const Uint64 now {SDL_GetTicksNS()};
            // "delta" = durée de la frame précédente, en SECONDES.
            // On divise par 1 milliard (1e9) car SDL_GetTicksNS() est en nanosecondes.
            // On multiplie les mouvements par delta pour qu'ils soient identiques
            // quel que soit le nombre de frames par seconde de la machine.
            const float delta {static_cast<float>(now - last) / 1'000'000'000.0f};
            last = now;

            process_events();      // 1. lire les entrées (clavier, souris, fermeture)
            world_.update(delta);  // 2. mettre à jour les entités
            update_camera(delta);  // 3. déplacer la caméra
            // L'UI n'écoute la souris qu'en mode curseur : en mode vol, la souris
            // appartient à la caméra (évite qu'un clic touche l'UI par accident).
            ui_.set_mouse_enabled(!fly_mode_);
            ui_.begin_frame();     // 4. démarrer l'interface de cette frame...
            build_ui();            //    ...et la construire
            render();              // 5. tout dessiner à l'écran
        }
    }

    // Lit tous les évènements en attente (une file remplie par le système).
    void application::process_events() {
        constexpr float look_sensitivity {0.005f}; // radians par pixel de souris

        SDL_Event event;
        // SDL_PollEvent retire un évènement de la file et renvoie faux quand elle est vide.
        while (SDL_PollEvent(&event)) {
            ui_.process_event(event); // ImGui voit chaque évènement en premier

            // On réagit selon le TYPE de l'évènement.
            switch (event.type) {
                case SDL_EVENT_QUIT:            // clic sur la croix / demande de fermeture
                    running_ = false;
                    break;
                case SDL_EVENT_KEY_DOWN:        // une touche est enfoncée
                    if (event.key.key == SDLK_ESCAPE) {
                        running_ = false;       // Échap ferme le jeu
                    } else if (event.key.key == SDLK_TAB) {
                        set_fly_mode(!fly_mode_); // Tab bascule vol <-> curseur (le "!" inverse le booléen)
                    }
                    break;
                case SDL_EVENT_MOUSE_MOTION:    // la souris a bougé
                    // On ne tourne la caméra qu'en mode vol ; en mode curseur, la
                    // souris sert à l'UI.
                    if (fly_mode_) {
                        // xrel/yrel = déplacement depuis la dernière frame. Le "-" inverse
                        // le sens pour que ça paraisse naturel.
                        camera_.look(-event.motion.xrel * look_sensitivity,
                                     -event.motion.yrel * look_sensitivity);
                    }
                    break;
                default:
                    break; // les autres évènements ne nous intéressent pas ici
            }
        }
    }

    void application::update_camera(const float delta_seconds) {
        // En mode curseur (UI), la caméra ne bouge pas : on sort tout de suite.
        if (!fly_mode_) {
            return;
        }

        // SDL_GetKeyboardState renvoie un tableau : keys[SCANCODE] vaut vrai si la
        // touche est enfoncée MAINTENANT (état continu, contrairement aux évènements).
        const bool* keys {SDL_GetKeyboardState(nullptr)};

        // On additionne une direction selon les touches pressées.
        // Repère caméra : x = droite, y = haut, z = avant.
        // Scancodes physiques : W/A/S/D tombent sur les touches Z/Q/S/D d'un AZERTY.
        glm::vec3 direction {0.0f};
        if (keys[SDL_SCANCODE_W]) { direction.z += 1.0f; } // avancer
        if (keys[SDL_SCANCODE_S]) { direction.z -= 1.0f; } // reculer
        if (keys[SDL_SCANCODE_D]) { direction.x += 1.0f; } // droite
        if (keys[SDL_SCANCODE_A]) { direction.x -= 1.0f; } // gauche
        if (keys[SDL_SCANCODE_SPACE])  { direction.y += 1.0f; } // monter
        if (keys[SDL_SCANCODE_LSHIFT]) { direction.y -= 1.0f; } // descendre

        // Si au moins une touche est pressée, on bouge. On normalise la direction
        // (longueur = 1) pour que se déplacer en diagonale n'aille pas plus vite.
        if (direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f) {
            camera_.move(glm::normalize(direction), delta_seconds);
        }
    }

    // Construit l'interface de debug de cette frame (une petite fenêtre ImGui).
    void application::build_ui() const {
        const ImGuiIO& io {ImGui::GetIO()}; // io contient des infos utiles (ex : FPS)

        // On calcule le point du SOL visé par la souris (picking). Utile surtout en
        // mode curseur (Tab) : c'est là qu'on posera bientôt les bâtiments.
        float mouse_x {0.0f};
        float mouse_y {0.0f};
        SDL_GetMouseState(&mouse_x, &mouse_y);   // position du curseur en pixels
        int width {0};
        int height {0};
        SDL_GetWindowSizeInPixels(window_.native(), &width, &height);
        const std::optional<glm::vec3> ground {camera_.pick_ground(
            {mouse_x, mouse_y}, {static_cast<float>(width), static_cast<float>(height)})};

        // Style "mode immédiat" : on décrit l'UI à chaque frame par des appels.
        ImGui::Begin("Cinder City");                                   // ouvre une fenêtre
        ImGui::Text("%.1f FPS", static_cast<double>(io.Framerate));    // images par seconde
        ImGui::Text("Bâtiments : %zu", world_.entities().size());      // nombre d'entités
        if (ground.has_value()) {                                      // has_value() : y a-t-il un point ?
            ImGui::Text("Souris au sol : (%.1f, %.1f)", ground->x, ground->z);
        } else {
            ImGui::Text("Souris au sol : —");                          // rayon parallèle au sol / vers le ciel
        }
        ImGui::End();                                                  // ferme la fenêtre
    }

    // Bascule entre "mode vol" et "mode curseur".
    void application::set_fly_mode(const bool enabled) {
        fly_mode_ = enabled;
        // La souris n'est capturée (relative) qu'en mode vol. En mode curseur, elle
        // redevient un curseur normal -> on peut cliquer dans les fenêtres ImGui.
        SDL_SetWindowRelativeMouseMode(window_.native(), enabled);
    }

    // Délègue tout le dessin au renderer, en lui passant la caméra, le monde et l'UI.
    void application::render() {
        renderer_.draw(camera_, world_, ui_);
    }
}
