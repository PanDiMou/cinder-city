// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

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

        // Remplace la LETTRE de variante (A/B/C) à la fin d'un nom de texture. Les
        // atlas Synty se déclinent en "..._01_A", "..._01_B", "..._01_C" (mêmes UV,
        // couleurs différentes) : ex "PolygonSunshineCity_Texture_01_A" + 'B' ->
        // "..._01_B". Si le nom ne finit pas par une telle lettre, on le laisse tel quel.
        std::string with_variant(std::string tex_name, const char letter) {
            if (!tex_name.empty()) {
                const char last {tex_name.back()};
                if (last == 'A' || last == 'B' || last == 'C') {
                    tex_name.back() = letter;
                }
            }
            return tex_name;
        }

        // Variantes de véhicule proposées à la pose. Deux tableaux PARALLÈLES (même
        // indice) : le libellé affiché, et la lettre d'atlas (A/B/C) réellement
        // stockée dans l'instance. Ce sont des recoloriages COMPLETS de l'atlas Synty
        // -> roues et vitres restent correctes (contrairement à une couleur unie, qui
        // repeindrait toute la voiture d'un bloc à cause du mono-matériau).
        constexpr const char* variant_labels[] {
            "Taxi", "Police", "Coastguard"
        };
        constexpr const char* variant_codes[] {
            "A", "B", "C"
        };
    }

    // Constructeur : prépare la scène de départ.
    application::application() {
        // On charge la liste des bâtiments depuis le fichier, puis on crée une entité
        // pour chacun. On protège avec try/catch : un fichier absent/invalide ne doit
        // pas faire planter le jeu, on démarre juste avec une scène vide.
        try {
            instances_ = load_scene(scene_path);
        } catch (const std::exception& error) {
            log::warning("Scène non chargée : {}", error.what());
        }
        rebuild_world();

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
                    // Clic gauche, en mode curseur, et PAS sur un panneau ImGui.
                    // Selon l'outil actif : poser un nouveau bâtiment, ou en sélectionner un.
                    if (event.button.button == SDL_BUTTON_LEFT && !fly_mode_ && !ui_.wants_mouse()) {
                        if (tool_ == tool::place) {
                            place_building(event.button.x, event.button.y);
                        } else {
                            select_building(event.button.x, event.button.y);
                        }
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
        if (direction != glm::vec3{0.0f}) {
            camera_.move(glm::normalize(direction), delta_seconds);
        }
    }

    // Construit l'interface de cette frame (fenêtre debug + palette de l'éditeur).
    void application::build_ui() {
        const ImGuiIO& io {ImGui::GetIO()}; // io contient des infos utiles (ex : FPS)

        // Point du sol visé par le curseur (pour l'affichage d'aide).
        float mouse_x {0.0f};
        float mouse_y {0.0f};
        SDL_GetMouseState(&mouse_x, &mouse_y);
        const std::optional<glm::vec3> ground {mouse_ground(mouse_x, mouse_y)};

        // Position/taille PAR DÉFAUT du panneau : ancré en haut-gauche, 340 px de
        // large. ImGuiCond_FirstUseEver = "seulement la toute première fois" : si
        // l'utilisateur déplace/redimensionne la fenêtre ensuite, on respecte son choix.
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(340.0f, 560.0f), ImGuiCond_FirstUseEver);

        // Style "mode immédiat" : on décrit l'UI à chaque frame par des appels.
        ImGui::Begin("Cinder City");                                   // ouvre une fenêtre

        // --- Section Infos (repliable). CollapsingHeader renvoie vrai si la section
        // est DÉPLIÉE ; DefaultOpen = ouverte au premier lancement. ---
        if (ImGui::CollapsingHeader("Infos", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("%.1f FPS", static_cast<double>(io.Framerate));    // images par seconde
            ImGui::Text("Bâtiments : %zu", instances_.size());            // posés (hors sol)
            if (ground) {
                ImGui::Text("Souris au sol : (%.1f, %.1f)", ground->x, ground->z);
            } else {
                ImGui::Text("Souris au sol : —");   // rayon parallèle au sol / vers le ciel
            }
        }

        // --- Section Outil : poser ou sélectionner ---
        if (ImGui::CollapsingHeader("Outil", ImGuiTreeNodeFlags_DefaultOpen)) {
            // RadioButton renvoie vrai au clic ; on change alors l'outil.
            if (ImGui::RadioButton("Placer", tool_ == tool::place)) {
                tool_ = tool::place;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Sélectionner", tool_ == tool::select)) {
                tool_ = tool::select;
            }
        }

        // --- Section Modèles : recherche + liste défilante ---
        if (ImGui::CollapsingHeader("Modèles", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Sélection : %s", selected_model_.c_str());

            // Barre de recherche : filtre la liste par son texte. "##" = identifiant
            // ImGui sans étiquette visible ; le "hint" est le texte grisé d'aide.
            ImGui::SetNextItemWidth(-1.0f);   // prend toute la largeur disponible
            ImGui::InputTextWithHint("##recherche", "rechercher...", search_, sizeof(search_));

            // Zone DÉFILANTE de hauteur fixe : la liste ne pousse pas le reste du
            // panneau, elle défile à l'intérieur (indispensable avec beaucoup de modèles).
            if (ImGui::BeginChild("liste_modeles", ImVec2(0.0f, 160.0f), ImGuiChildFlags_Borders)) {
                const std::string filter {search_};   // texte tapé (peut être vide)
                for (const std::string& model : model_list_) {
                    // find() renvoie npos si le filtre n'apparaît pas dans le nom ->
                    // on saute ce modèle. Filtre vide = tout passe.
                    if (!filter.empty() && model.find(filter) == std::string::npos) {
                        continue;
                    }
                    if (ImGui::Selectable(model.c_str(), selected_model_ == model)) {
                        selected_model_ = model;
                    }
                }
            }
            ImGui::EndChild();   // TOUJOURS appelé, même si BeginChild a renvoyé faux

            // Le select de variante n'a de sens que pour les véhicules (livrée
            // Taxi/Police/Coastguard) : on ne l'affiche donc que si le modèle
            // sélectionné en est un (nom qui commence par "SM_Veh").
            if (selected_model_.rfind("SM_Veh", 0) == 0) {
                ImGui::Combo("Variante", &variant_index_, variant_labels, IM_ARRAYSIZE(variant_labels));
            }

            ImGui::TextWrapped("Tab pour le curseur, puis clic gauche sur le sol.");
        }

        // --- Section Bâtiment sélectionné (visible seulement si sélection valide) ---
        if (selected_index_ >= 0 && selected_index_ < static_cast<int>(instances_.size())
            && ImGui::CollapsingHeader("Bâtiment sélectionné", ImGuiTreeNodeFlags_DefaultOpen)) {
            scene_instance& selected {instances_[static_cast<std::size_t>(selected_index_)]};

            ImGui::Text("%s", selected.model.c_str());

            // DragFloat(3) = un champ qu'on modifie en glissant la souris dessus.
            // Chacun renvoie vrai quand la valeur change ; on cumule dans "changed".
            bool changed {false};
            changed |= ImGui::DragFloat3("Position", &selected.position.x, 0.1f);
            changed |= ImGui::DragFloat("Rotation Y", &selected.rotation_y, 1.0f);
            changed |= ImGui::DragFloat("Échelle", &selected.scale, 0.01f, 0.1f, 10.0f);

            if (ImGui::Button("Pivoter +90°")) {
                selected.rotation_y += 90.0f;
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Supprimer")) {
                // erase enlève l'élément de la liste ; begin() + i = position de l'élément.
                instances_.erase(instances_.begin() + selected_index_);
                selected_index_ = -1;
                rebuild_world();          // le monde doit refléter la liste modifiée
            } else if (changed) {
                rebuild_world();          // une valeur a changé : on reconstruit le monde
            }
        }

        // --- Section Scène : sauvegarde / rechargement ---
        if (ImGui::CollapsingHeader("Scène", ImGuiTreeNodeFlags_DefaultOpen)) {
            // ImGui::Button renvoie vrai la frame où on clique dessus.
            if (ImGui::Button("Sauvegarder")) {
                save();
            }
            ImGui::SameLine();   // met le bouton suivant sur la MÊME ligne
            if (ImGui::Button("Recharger")) {
                reload();
            }
        }

        ImGui::End();                                                  // ferme la fenêtre
    }

    // Point du sol visé par le curseur, ou rien. Regroupe le calcul de picking.
    std::optional<glm::vec3> application::mouse_ground(const float mouse_x, const float mouse_y) const {
        int width {0};
        int height {0};
        SDL_GetWindowSizeInPixels(window_.native(), &width, &height);
        return camera_.pick_ground({mouse_x, mouse_y},
                                   {static_cast<float>(width), static_cast<float>(height)});
    }

    // (Re)crée le sol dans le monde. Le sol n'est pas une "instance" sauvegardée :
    // il est toujours là, on le recrée nous-mêmes (au démarrage et au rechargement).
    void application::spawn_ground() {
        // Le sol de référence est légèrement ENFONCÉ (y = -0.05). Sinon les dalles de
        // route Synty, parfaitement plates (épaisseur 0, posées à y=0), tomberaient
        // dans le MÊME plan que ce sol -> "z-fighting" : le GPU ne sait pas laquelle
        // est devant, la route clignote et paraît invisible ("impossible à placer").
        transform ground_transform;
        ground_transform.position.y = -0.05f;
        world_.spawn<static_prop>(ground_mesh_, ground_transform, glm::vec4{0.30f, 0.30f, 0.32f, 1.0f}, material_type::grid_floor);
    }

    // Transforme une instance (donnée) en entité visible dans le monde.
    void application::spawn_instance(const scene_instance& instance) {
        transform transform;
        transform.position = instance.position;
        // angleAxis fabrique une rotation d'un angle donné autour d'un axe (ici Y, vertical).
        transform.rotation = glm::angleAxis(glm::radians(instance.rotation_y), glm::vec3{0.0f, 1.0f, 0.0f});
        transform.scale = glm::vec3{instance.scale};

        // On charge la géométrie (le catalogue relève au passage le nom de sa texture).
        const gpu_mesh& mesh {catalog_.get(instance.model)};

        // Livrée texturée : on part de la texture du modèle et, si l'instance demande
        // une variante A/B/C, on remplace la lettre de l'atlas en conséquence.
        std::string tex_name {catalog_.texture_name(instance.model)};
        if (!instance.variant.empty()) {
            tex_name = with_variant(tex_name, instance.variant.front());
        }
        // Nom vide -> nullptr : le renderer se rabattra sur la palette globale.
        const texture* tex {tex_name.empty() ? nullptr : &textures_.get(tex_name)};

        world_.spawn<static_prop>(mesh, transform, glm::vec4{1.0f}, material_type::textured, tex);
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
        try {
            instances_ = load_scene(scene_path);   // relit la liste depuis le disque
        } catch (const std::exception& error) {
            log::error("Échec du rechargement : {}", error.what());
            instances_.clear();
        }
        selected_index_ = -1;   // l'ancienne sélection n'a plus de sens
        rebuild_world();        // reconstruit le sol + tous les bâtiments
    }

    // Pose le bâtiment sélectionné à l'endroit du sol pointé par (mouse_x, mouse_y).
    void application::place_building(const float mouse_x, const float mouse_y) {
        // Si le rayon ne touche pas le sol (clic vers le ciel), on ne pose rien.
        const std::optional ground {mouse_ground(mouse_x, mouse_y)};
        if (!ground) {
            return;
        }

        // On ajoute d'abord l'instance à la liste (la source de vérité, sauvegardable),
        // puis on la matérialise dans le monde. *ground déréférence l'optional (le point).
        scene_instance instance {.model = selected_model_, .position = *ground};
        // On n'applique la variante qu'aux véhicules (le select n'apparaît que pour
        // eux) : les bâtiments/routes gardent ainsi leur apparence par défaut.
        if (selected_model_.rfind("SM_Veh", 0) == 0) {
            instance.variant = variant_codes[variant_index_];   // livrée/couleur choisie dans l'IHM
        }
        instances_.push_back(instance);
        spawn_instance(instance);
    }

    // Sélectionne le bâtiment le plus proche du point cliqué (dans un certain rayon).
    void application::select_building(const float mouse_x, const float mouse_y) {
        const std::optional ground {mouse_ground(mouse_x, mouse_y)};
        if (!ground) {
            return;
        }

        // Rayon de sélection : on ne sélectionne que si un bâtiment est assez proche.
        // On compare des distances AU CARRÉ pour éviter une racine carrée (plus rapide).
        constexpr float radius {8.0f};
        float best_distance2 {radius * radius};
        int best {-1};

        for (std::size_t i {0}; i < instances_.size(); ++i) {
            const glm::vec3& position {instances_[i].position};
            const float dx {position.x - ground->x};
            const float dz {position.z - ground->z};
            if (const float distance2 {dx * dx + dz * dz}; distance2 < best_distance2) {
                best_distance2 = distance2;
                best = static_cast<int>(i);
            }
        }
        selected_index_ = best;   // -1 si aucun bâtiment n'était assez proche
    }

    // Reconstruit entièrement le monde à partir de la liste d'instances (source de vérité).
    // Appelé après une modification (déplacer, tourner, supprimer). Bon marché : les
    // géométries sont déjà en cache dans le catalogue, on ne recrée que les entités.
    void application::rebuild_world() {
        world_.clear();
        spawn_ground();
        for (const scene_instance& instance : instances_) {
            spawn_instance(instance);
        }
    }

    // Bascule entre "mode vol" et "mode curseur".
    void application::set_fly_mode(const bool enabled) {
        fly_mode_ = enabled;
        // La souris n'est capturée (relative) qu'en mode vol. En mode curseur, elle
        // redevient un curseur normal -> on peut cliquer dans les fenêtres ImGui.
        SDL_SetWindowRelativeMouseMode(window_.native(), enabled);
        ui_.set_mouse_enabled(!enabled);   // l'UI n'écoute la souris qu'en mode curseur
    }

    // Délègue tout le dessin au renderer, en lui passant la caméra, le monde et l'UI.
    void application::render() {
        renderer_.draw(camera_, world_, ui_);
    }
}
