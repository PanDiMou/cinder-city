// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// gpu_mesh — la géométrie prête à dessiner, DANS le GPU.
//
// Alors que `mesh` vit en RAM, `gpu_mesh` regroupe deux gpu_buffer (un pour les
// sommets, un pour les indices) déjà envoyés en VRAM, plus le nombre d'indices à
// dessiner. Sa méthode bind_and_draw() dit au GPU "utilise cette géométrie et dessine-la".
//
// "Partagée" : une seule gpu_mesh peut être pointée par plein d'entités (le
// catalogue s'en sert pour ne charger un modèle qu'une fois).
// ============================================================================

#ifndef CINDER_CITY_GPU_MESH_HPP
#define CINDER_CITY_GPU_MESH_HPP

#include "engine/render/gpu_buffer.hpp"

#include <cstdint>

struct SDL_GPURenderPass;   // déclaration anticipée

namespace cinder {
    class graphics_device;
    struct mesh;

    class gpu_mesh {
    public:
        // Construit la géométrie GPU à partir d'un mesh (RAM). Le constructeur
        // uploade les sommets et indices en VRAM (via gpu_buffer).
        gpu_mesh(const graphics_device&, const mesh&);

        gpu_mesh(const gpu_mesh&) = delete;              // possède des buffers GPU -> pas de copie
        gpu_mesh& operator=(const gpu_mesh&) = delete;

        // Attache cette géométrie à la passe de rendu et lance le dessin.
        void bind_and_draw(SDL_GPURenderPass*) const;

    private:
        gpu_buffer vertices_;          // les sommets, en VRAM
        gpu_buffer indices_;           // les indices, en VRAM
        std::uint32_t index_count_ {0}; // combien d'indices dessiner (3 par triangle)
    };
}

#endif //CINDER_CITY_GPU_MESH_HPP
