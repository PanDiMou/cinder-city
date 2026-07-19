// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// graphics_device — la "carte graphique" vue par le programme (le GPU).
//
// C'est le point d'accès à SDL_gpu (l'API de rendu moderne de SDL). Toutes les
// ressources graphiques (buffers, textures, shaders, pipelines) sont créées à
// partir de ce device. On le crée une fois au démarrage, et tout le module
// render/ s'appuie dessus.
// ============================================================================

#ifndef CINDER_CITY_GRAPHICS_DEVICE_HPP
#define CINDER_CITY_GRAPHICS_DEVICE_HPP

struct SDL_GPUDevice;   // déclarations anticipées (manipulés par pointeur)
struct SDL_Window;

namespace cinder {
    class window;

    class graphics_device {
    public:
        // Crée le GPU et l'associe à la fenêtre. "explicit" : pas de conversion
        // automatique d'une window en graphics_device.
        explicit graphics_device(window&);

        graphics_device(const graphics_device&) = delete;              // ressource unique -> pas de copie
        graphics_device& operator=(const graphics_device&) = delete;

        ~graphics_device();   // libère le GPU

        // native() : le SDL_GPUDevice brut, demandé par toutes les ressources GPU.
        [[nodiscard]] SDL_GPUDevice* native() const noexcept {
            return device_;
        }

        // window_handle() : la fenêtre associée (utile pour la swapchain, l'UI...).
        [[nodiscard]] SDL_Window* window_handle() const noexcept {
            return window_;
        }

    private:
        SDL_GPUDevice* device_ {nullptr};   // le GPU
        SDL_Window* window_ {nullptr};      // la fenêtre à laquelle il est lié
    };
}

#endif //CINDER_CITY_GRAPHICS_DEVICE_HPP
