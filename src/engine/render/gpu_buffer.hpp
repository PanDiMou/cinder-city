// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// gpu_buffer — un bloc de mémoire VIDÉO (dans le GPU).
//
// Le GPU a sa propre mémoire (la VRAM). Pour dessiner, il faut y COPIER les
// données depuis la mémoire normale (RAM). Un gpu_buffer représente un tel bloc
// dans la VRAM. On s'en sert pour deux choses : stocker les sommets, ou stocker
// les indices (d'où l'enum buffer_usage).
//
// RAII : la copie vers la VRAM se fait à la construction, la libération à la
// destruction.
// ============================================================================

#ifndef CINDER_CITY_GPU_BUFFER_HPP
#define CINDER_CITY_GPU_BUFFER_HPP

#include <cstdint>

// Types SDL manipulés seulement par pointeur ici -> déclarations anticipées.
struct SDL_GPUDevice;
struct SDL_GPUBuffer;

namespace cinder {
    class graphics_device;

    // À quoi sert ce buffer : contenir des sommets ou des indices.
    enum class buffer_usage {
        vertex,
        index
    };

    class gpu_buffer {
    public:
        // Construit le buffer ET copie les données dedans.
        //  - data  : pointeur vers les données (const void* = "adresse d'octets quelconques")
        //  - size  : taille en octets
        //  - usage : sommets ou indices
        gpu_buffer(const graphics_device& device, const void* data, std::uint32_t size, buffer_usage usage);

        gpu_buffer(const gpu_buffer&) = delete;              // possède une ressource GPU -> pas de copie
        gpu_buffer& operator=(const gpu_buffer&) = delete;

        ~gpu_buffer();   // libère le buffer GPU

        // Donne le buffer SDL brut (pour les fonctions de dessin).
        [[nodiscard]] SDL_GPUBuffer* native() const noexcept { return buffer_; }

    private:
        SDL_GPUDevice* device_ {nullptr};   // le GPU (pour créer/libérer)
        SDL_GPUBuffer* buffer_ {nullptr};   // le bloc de VRAM lui-même
    };
}

#endif //CINDER_CITY_GPU_BUFFER_HPP
