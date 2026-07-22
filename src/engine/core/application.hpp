// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

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
#include "engine/assets/texture_catalog.hpp"
#include "engine/scene/camera.hpp"
#include "engine/editor/ui.hpp"
#include "engine/scene/scene_loader.hpp"   // pour scene_instance
#include "engine/world/ground.hpp"
#include "engine/world/world.hpp"

#include <optional>
#include <string>
#include <vector>

#include <glm/glm.hpp>

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
        void select_building(float mouse_x, float mouse_y); // sélectionne le bâtiment le plus proche du clic
        void spawn_ground();                 // (re)crée le sol dans le monde
        void spawn_instance(const scene_instance& instance); // crée l'entité d'une instance
        void rebuild_world();                // reconstruit le monde à partir des instances_
        void save() const;                   // écrit la scène courante dans city.json
        void reload();                       // vide le monde et recharge city.json
        void render();                       // dessine le monde + l'UI à l'écran

        // Point du sol visé par le curseur (ou rien si le rayon manque le sol).
        // Regroupe le code de picking utilisé à plusieurs endroits.
        [[nodiscard]] std::optional<glm::vec3> mouse_ground(float mouse_x, float mouse_y) const;

        // L'outil actif de l'éditeur : poser un nouveau bâtiment, ou en sélectionner un.
        enum class tool { place, select };

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
        texture_catalog textures_ {graphics_device_};           // charge les textures une seule fois, à la demande
        world           world_;                                 // contient toutes les entités de la scène
        camera          camera_;                                // le point de vue
        ui              ui_ {graphics_device_};                 // l'interface (Dear ImGui)
        bool            fly_mode_ {true};                       // true : la souris pilote la caméra ; false : elle pilote l'UI (Tab pour basculer)
        bool            running_ {true};                        // devient false pour arrêter la boucle

        // Palette de l'éditeur : les modèles qu'on peut poser, et celui sélectionné.
        // Ici, les bâtiments d'arrière-plan Synty. Ils utilisent TOUS l'atlas
        // "Generic_01_A" -> c'est celui que le renderer charge comme palette globale.
        std::vector<std::string> model_list_ {
            "SM_Gen_Bld_Background_01",
            "SM_Gen_Bld_Background_02",
            "SM_Gen_Bld_Background_03",
            "SM_Gen_Bld_Background_04",
            "SM_Gen_Bld_Background_05",
            "SM_Gen_Bld_Background_06",
            "SM_Gen_Bld_Background_07",
            "SM_Gen_Bld_Background_08",
            "SM_Gen_Bld_Background_09",
            "SM_Gen_Bld_Background_10",
            "SM_Gen_Bld_Background_11",
            // Kit route/trottoir Generic (texture Generic_Road_01, résolue par modèle).
            "SM_Gen_Env_Road_01",
            "SM_Gen_Env_Road_Half_01",
            "SM_Gen_Env_Road_Small_01",
            "SM_Gen_Env_Road_Crossing_01",
            "SM_Gen_Env_Road_Intersection_01",
            "SM_Gen_Env_Road_Parking_01",
            "SM_Gen_Env_Sidewalk_01",
            "SM_Gen_Env_Sidewalk_Corner_01",
            "SM_Gen_Env_Sidewalk_Half_01",
            // Véhicule : arbre complet (carrosserie, vitres, roues, portes), texture
            // carrosserie PolygonPalmCity (via l'alias SunshineCity -> PalmCity).
            "SM_Veh_Sedan_01"
        };
        std::string              selected_model_ {"SM_Gen_Bld_Background_01"};
        // Indice de la variante choisie dans l'IHM (0 = "Défaut"). Appliquée au
        // prochain modèle posé : livrée A/B/C (véhicules) ou couleur unie.
        int                      variant_index_ {0};
        // Tampon de la barre de recherche de la palette. ImGui écrit dedans
        // directement (tableau de char à la C, taille fixe, terminé par '\0').
        char                     search_[128] {};

        // La scène en mémoire : la liste des bâtiments posés. C'est la SOURCE DE
        // VÉRITÉ qu'on sauvegarde. Le monde (world_) en est le reflet visible.
        std::vector<scene_instance> instances_;

        tool tool_ {tool::place};   // outil courant de l'éditeur
        int  selected_index_ {-1};  // indice du bâtiment sélectionné dans instances_ (-1 = aucun)
    };
}

#endif //CINDER_CITY_APPLICATION_HPP
