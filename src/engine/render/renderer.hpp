// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

// ============================================================================
// renderer — celui qui DESSINE le monde à l'écran, à chaque frame.
//
// Il possède les "pipelines" graphiques (un pipeline = une configuration
// complète du GPU pour un type de rendu : quels shaders, comment mélanger, etc.).
// Il y en a un par matériau (couleur unie, sol quadrillé, texturé).
//
// Sa méthode draw() est appelée chaque frame : elle parcourt le monde et dessine
// chaque entité avec le bon pipeline, puis dessine l'interface (ImGui) par-dessus.
// ============================================================================

#ifndef CINDER_CITY_RENDERER_HPP
#define CINDER_CITY_RENDERER_HPP

#include "engine/render/texture.hpp"   // inclus car `texture palette_` est un membre par valeur

struct SDL_GPUDevice;             // déclarations anticipées (manipulés par pointeur)
struct SDL_Window;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUTexture;

namespace cinder {
    class graphics_device;
    class camera;
    class world;
    class ui;

    class renderer {
    public:
        explicit renderer(const graphics_device&);   // crée les pipelines et le depth buffer
        renderer(const renderer&) = delete;              // possède des ressources GPU -> pas de copie
        renderer& operator=(const renderer&) = delete;

        ~renderer();

        // Dessine une frame complète : le monde vu par la caméra, puis l'UI.
        // Les paramètres sont des références "const" (lecture seule) sauf ui, que
        // le rendu ImGui modifie.
        void draw(const camera&, const world&, ui&) const;

    private:
        SDL_GPUDevice* device_ {nullptr};   // le GPU
        SDL_Window* window_ {nullptr};      // la fenêtre (pour récupérer l'image à afficher)

        // Un pipeline par matériau : le renderer choisit le bon selon l'entité.
        SDL_GPUGraphicsPipeline* solid_color_pipeline_ {nullptr}; // matériau couleur unie
        SDL_GPUGraphicsPipeline* grid_floor_pipeline_ {nullptr};  // matériau sol quadrillé
        SDL_GPUGraphicsPipeline* textured_pipeline_ {nullptr};    // matériau texturé par palette

        SDL_GPUTexture* depth_ {nullptr};   // "buffer de profondeur" : gère quel objet est devant l'autre
        texture palette_;                   // la palette de couleurs partagée par les modèles
        texture emissive_;                  // l'émissif (fenêtres/enseignes allumées), ajouté par-dessus
    };
}

#endif //CINDER_CITY_RENDERER_HPP
