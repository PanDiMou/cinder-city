// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

// ============================================================================
// ui — l'interface à l'écran (panneaux, boutons...), via Dear ImGui.
//
// ImGui est en "mode immédiat" : on ne construit pas des boutons une fois pour
// toutes, on redécrit toute l'interface À CHAQUE frame par des appels de fonctions.
// Cette classe cache la mécanique d'ImGui derrière quelques méthodes simples, et
// gère sa création/destruction en RAII.
//
// Enchaînement dans une frame :
//   process_event() (pour chaque évènement) -> begin_frame() -> [décrire l'UI]
//   -> finalize() -> upload() -> render().
// ============================================================================

#ifndef CINDER_CITY_UI_HPP
#define CINDER_CITY_UI_HPP

struct SDL_GPUDevice;          // déclarations anticipées
struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;
union SDL_Event;               // note : SDL_Event est une "union", pas un "struct"

namespace cinder {
    class graphics_device;

    class ui {
    public:
        explicit ui(const graphics_device& device);   // initialise ImGui + ses backends
        ui(const ui&) = delete;              // contexte global unique -> pas de copie
        ui& operator=(const ui&) = delete;

        ~ui();   // démonte ImGui

        void process_event(const SDL_Event& event);  // transmet un évènement SDL à ImGui (clics, clavier)
        void set_mouse_enabled(bool enabled);         // active/désactive la prise en compte de la souris par l'UI
        void begin_frame();                           // démarre une nouvelle frame ImGui
        void finalize();                              // clôt la frame (ImGui::Render)
        void upload(SDL_GPUCommandBuffer* command_buffer);                          // prépare les données de dessin (VRAM)
        void render(SDL_GPUCommandBuffer* command_buffer, SDL_GPURenderPass* pass); // dessine l'UI dans la passe

        // Vrai si la souris survole/utilise un panneau ImGui. Sert à ne PAS poser
        // de bâtiment quand on clique sur l'interface plutôt que sur le sol.
        [[nodiscard]] bool wants_mouse() const;

    private:
        SDL_GPUDevice* device_ {nullptr};
    };
}

#endif //CINDER_CITY_UI_HPP
