// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// texture — une image chargée dans le GPU, plus la façon de la lire (sampler).
//
// Deux objets distincts, souvent confondus :
//   - la TEXTURE : les pixels de l'image, stockés en VRAM.
//   - le SAMPLER : les règles de lecture (filtrage, répétition aux bords...).
//
// Ici on charge une image (PNG) via stb_image, on l'envoie au GPU, et on crée
// un sampler adapté. Les shaders s'en servent pour colorer les modèles (palette Synty).
// ============================================================================

#ifndef CINDER_CITY_TEXTURE_HPP
#define CINDER_CITY_TEXTURE_HPP

#include <string>

struct SDL_GPUDevice;    // déclarations anticipées
struct SDL_GPUTexture;
struct SDL_GPUSampler;

namespace cinder {
    class graphics_device;

    class texture {
    public:
        // Charge une image depuis un fichier et l'envoie au GPU.
        texture(const graphics_device&, const std::string&);

        texture(const texture&) = delete;              // ressources GPU -> pas de copie
        texture& operator=(const texture&) = delete;

        ~texture();

        // Accès à la texture et au sampler bruts (pour les lier au moment du dessin).
        [[nodiscard]] SDL_GPUTexture* native() const noexcept { return texture_; }
        [[nodiscard]] SDL_GPUSampler* sampler() const noexcept { return sampler_; }

    private:
        SDL_GPUDevice*  device_  {nullptr};
        SDL_GPUTexture* texture_ {nullptr};   // les pixels en VRAM
        SDL_GPUSampler* sampler_ {nullptr};   // les règles de lecture
    };
}

#endif //CINDER_CITY_TEXTURE_HPP
