// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// application — le chef d'orchestre du jeu.
//
// C'est ici que tout se rejoint : la fenêtre, le GPU, la scène, la caméra, l'UI.
// La méthode run() contient la "boucle de jeu" : lire les entrées -> mettre à
// jour -> dessiner, répété des dizaines de fois par seconde jusqu'à la fermeture.
//
// L'ORDRE des membres tout en bas est important : ils sont construits de haut en
// bas, et détruits dans l'ordre inverse. Ex : `platform` (SDL) doit exister
// AVANT `window`, et être détruit APRÈS. On respecte donc les dépendances.
// ============================================================================

#ifndef CINDER_CITY_APPLICATION_HPP
#define CINDER_CITY_APPLICATION_HPP

#include "engine/core/platform.hpp"
#include "engine/core/window.hpp"
#include "engine/render/graphics_device.hpp"
#include "engine/render/renderer.hpp"
#include "engine/render/gpu_mesh.hpp"
#include "engine/assets/model_catalog.hpp"
#include "engine/scene/camera.hpp"
#include "engine/editor/ui.hpp"
#include "engine/scene/scene_loader.hpp"   // pour scene_instance
#include "engine/world/ground.hpp"
#include "engine/world/world.hpp"

#include <string>
#include <vector>

namespace cinder {
    class application {
    public:
        application();   // met tout en place (fenêtre, GPU, chargement de la scène)

        application(const application&) = delete;              // pas de copie : c'est un objet unique
        application& operator=(const application&) = delete;

        void run();      // lance la boucle de jeu (bloque jusqu'à la fermeture)

    private:
        // Ces méthodes privées découpent le travail d'une frame (une image).
        void process_events();               // lit clavier/souris/fermeture
        void update_camera(float delta_seconds); // déplace la caméra selon les touches
        void build_ui();                     // construit l'interface ImGui de la frame
        void set_fly_mode(bool enabled);     // bascule "vol caméra" <-> "curseur UI"
        void place_building(float mouse_x, float mouse_y); // pose le modèle choisi sous le curseur
        void spawn_ground();                 // (re)crée le sol dans le monde
        void spawn_instance(const scene_instance& instance); // crée l'entité d'une instance
        void save() const;                         // écrit la scène courante dans city.json
        void reload();                       // vide le monde et recharge city.json
        void render();                       // dessine le monde + l'UI à l'écran

        // --- Les membres : les briques qui composent le jeu ---
        // Rappel : ils sont initialisés dans cet ordre. Certains ont besoin d'un
        // autre pour se construire, d'où la syntaxe {membre_dont_ils_dependent}.
        platform        platform_;                              // démarre/arrête SDL
        window          window_;                                // la fenêtre
        graphics_device graphics_device_ {window_};             // le GPU (a besoin de la fenêtre)
        renderer        renderer_ {graphics_device_};           // dessine (a besoin du GPU)
        ground          ground_;                                // la géométrie du sol
        gpu_mesh        ground_mesh_ {graphics_device_, ground_.geometry()}; // le sol envoyé au GPU
        model_catalog   catalog_ {graphics_device_};            // charge les modèles FBX une seule fois, à la demande
        world           world_;                                 // contient toutes les entités de la scène
        camera          camera_;                                // le point de vue
        ui              ui_ {graphics_device_};                 // l'interface (Dear ImGui)
        bool            fly_mode_ {true};                       // true : la souris pilote la caméra ; false : elle pilote l'UI (Tab pour basculer)
        bool            running_ {true};                        // devient false pour arrêter la boucle

        // Palette de l'éditeur : les modèles qu'on peut poser, et celui sélectionné.
        // (Ils partagent tous la même texture de palette que celle chargée par le renderer.)
        std::vector<std::string> model_list_ {
            "SM_Bld_Beach_Shop_01",
            "SM_Bld_Beach_Shop_02",
            "SM_Bld_Beach_Shop_03"
        };
        std::string              selected_model_ {"SM_Bld_Beach_Shop_01"};

        // La scène en mémoire : la liste des bâtiments posés. C'est la SOURCE DE
        // VÉRITÉ qu'on sauvegarde. Le monde (world_) en est le reflet visible.
        std::vector<scene_instance> instances_;
    };
}

#endif //CINDER_CITY_APPLICATION_HPP
