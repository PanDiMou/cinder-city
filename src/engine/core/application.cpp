// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "application.hpp"
#include "engine/world/static_prop.hpp"
#include "engine/scene/transform.hpp"
#include "engine/core/log.hpp"

#include <glm/gtc/quaternion.hpp>   // glm::angleAxis
#include <imgui.h>
#include <SDL3/SDL.h>

namespace cinder {
    namespace {
        // Le fichier de scène : on le lit au démarrage et on l'écrit à la sauvegarde.
        constexpr const char* scene_path {"assets/city.json"};
    }

    // Constructeur : prépare la scène de départ.
    application::application() {
        spawn_ground();   // le sol quadrillé (il ne fait pas partie des instances sauvegardées)

        // On charge la liste des bâtiments depuis le fichier, puis on crée une entité
        // pour chacun. On protège avec try/catch : un fichier absent/invalide ne doit
        // pas faire planter le jeu, on démarre juste avec une scène vide.
        try {
            instances_ = load_scene(scene_path);
        } catch (const std::exception& error) {
            log::warning("Scène non chargée : {}", error.what());
        }
        for (const scene_instance& instance : instances_) {
            spawn_instance(instance);
        }

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
                case SDL_EVENT_MOUSE_BUTTON_DOWN:   // un bouton de souris est cliqué
                    // Clic gauche, en mode curseur, et PAS sur un panneau ImGui
                    // (wants_mouse) -> on pose le bâtiment choisi à cet endroit du sol.
                    if (event.button.button == SDL_BUTTON_LEFT && !fly_mode_ && !ui_.wants_mouse()) {
                        place_building(event.button.x, event.button.y);
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

    // Construit l'interface de cette frame (fenêtre debug + palette de l'éditeur).
    void application::build_ui() {
        const ImGuiIO& io {ImGui::GetIO()}; // io contient des infos utiles (ex : FPS)

        // On calcule le point du SOL visé par la souris (picking). C'est là qu'un
        // clic posera le bâtiment sélectionné.
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
        ImGui::Text("Bâtiments : %zu", instances_.size());            // nombre de bâtiments posés (hors sol)
        if (ground.has_value()) {                                      // has_value() : y a-t-il un point ?
            ImGui::Text("Souris au sol : (%.1f, %.1f)", ground->x, ground->z);
        } else {
            ImGui::Text("Souris au sol : —");                          // rayon parallèle au sol / vers le ciel
        }

        // --- Palette de l'éditeur : la liste des modèles qu'on peut poser ---
        ImGui::SeparatorText("Modèles");
        for (const std::string& model : model_list_) {
            // ImGui::Selectable dessine une ligne cliquable. Son 2e argument dit si
            // elle est "sélectionnée" (surlignée). Elle renvoie vrai au clic ->
            // on met alors ce modèle comme sélection courante.
            if (ImGui::Selectable(model.c_str(), selected_model_ == model)) {
                selected_model_ = model;
            }
        }
        ImGui::TextWrapped("Tab pour le curseur, puis clic gauche sur le sol pour poser.");

        // --- Sauvegarde / rechargement de la scène ---
        ImGui::SeparatorText("Scène");
        // ImGui::Button renvoie vrai la frame où on clique dessus.
        if (ImGui::Button("Sauvegarder")) {
            save();
        }
        ImGui::SameLine();   // met le bouton suivant sur la MÊME ligne
        if (ImGui::Button("Recharger")) {
            reload();
        }

        ImGui::End();                                                  // ferme la fenêtre
    }

    // (Re)crée le sol dans le monde. Le sol n'est pas une "instance" sauvegardée :
    // il est toujours là, on le recrée nous-mêmes (au démarrage et au rechargement).
    void application::spawn_ground() {
        world_.spawn<static_prop>(ground_mesh_, transform{}, glm::vec4{0.30f, 0.30f, 0.32f, 1.0f}, material_type::grid_floor);
    }

    // Transforme une instance (donnée) en entité visible dans le monde.
    void application::spawn_instance(const scene_instance& instance) {
        transform transform;
        transform.position = instance.position;
        // angleAxis fabrique une rotation d'un angle donné autour d'un axe (ici Y, vertical).
        transform.rotation = glm::angleAxis(glm::radians(instance.rotation_y), glm::vec3{0.0f, 1.0f, 0.0f});
        transform.scale = glm::vec3{instance.scale};

        world_.spawn<static_prop>(catalog_.get(instance.model), transform, glm::vec4{1.0f}, material_type::textured);
    }

    // Écrit la scène courante (la liste d'instances) dans le fichier city.json.
    void application::save() const {
        try {
            save_scene(scene_path, instances_);
            log::info("Scène sauvegardée ({} bâtiments)", instances_.size());
        } catch (const std::exception& error) {
            log::error("Échec de la sauvegarde : {}", error.what());
        }
    }

    // Vide le monde et le reconstruit depuis le fichier (annule les poses non sauvées).
    void application::reload() {
        world_.clear();     // supprime toutes les entités (sol compris)
        spawn_ground();     // on recrée le sol
        try {
            instances_ = load_scene(scene_path);   // relit la liste depuis le disque
        } catch (const std::exception& error) {
            log::error("Échec du rechargement : {}", error.what());
            instances_.clear();
        }
        for (const scene_instance& instance : instances_) {
            spawn_instance(instance);
        }
    }

    // Pose le bâtiment sélectionné à l'endroit du sol pointé par (mouse_x, mouse_y).
    void application::place_building(const float mouse_x, const float mouse_y) {
        int width {0};
        int height {0};
        SDL_GetWindowSizeInPixels(window_.native(), &width, &height);

        // On retrouve le point du sol sous le clic. Si le rayon ne touche pas le
        // sol (clic vers le ciel), on ne pose rien.
        const std::optional<glm::vec3> ground {camera_.pick_ground(
            {mouse_x, mouse_y}, {static_cast<float>(width), static_cast<float>(height)})};
        if (!ground.has_value()) {
            return;
        }

        // On ajoute d'abord l'instance à la liste (la source de vérité, sauvegardable),
        // puis on la matérialise dans le monde. *ground déréférence l'optional (le point).
        const scene_instance instance {.model = selected_model_, .position = *ground};
        instances_.push_back(instance);
        spawn_instance(instance);
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
