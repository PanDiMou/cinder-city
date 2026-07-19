// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "gpu_mesh.hpp"
#include "engine/render/mesh.hpp"

#include <SDL3/SDL.h>

namespace cinder {
    // Constructeur : il initialise ses trois membres dans la liste après ":".
    // Pour chaque buffer, la taille en OCTETS = (nombre d'éléments) x (taille d'un élément).
    // .data() donne l'adresse du premier élément du tableau ; .size() son nombre d'éléments.
    gpu_mesh::gpu_mesh(const graphics_device& device, const mesh& mesh):
        vertices_ {device, mesh.vertices.data(), static_cast<std::uint32_t>(mesh.vertices.size() * sizeof(vertex)), buffer_usage::vertex},
        indices_ {device, mesh.indices.data(), static_cast<std::uint32_t>(mesh.indices.size() * sizeof(std::uint32_t)), buffer_usage::index},
        index_count_ {static_cast<std::uint32_t>(mesh.indices.size())} { }

    void gpu_mesh::bind_and_draw(SDL_GPURenderPass* pass) const {
        // "Binder" = dire au GPU "voici les données à utiliser pour le prochain dessin".
        // On attache d'abord le buffer de sommets (au slot 0)...
        const SDL_GPUBufferBinding vertex_binding {.buffer = vertices_.native(), .offset = 0};
        SDL_BindGPUVertexBuffers(pass, 0, &vertex_binding, 1);

        // ...puis le buffer d'indices (indices sur 32 bits, comme nos std::uint32_t).
        const SDL_GPUBufferBinding index_binding {.buffer = indices_.native(), .offset = 0};
        SDL_BindGPUIndexBuffer(pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        // Enfin, on lance le dessin : "dessine index_count_ indices" (soit
        // index_count_ / 3 triangles). Les autres paramètres (1, 0, 0, 0) servent
        // à des cas avancés (instances, décalages) qu'on n'utilise pas encore.
        SDL_DrawGPUIndexedPrimitives(pass, index_count_, 1, 0, 0, 0);
    }
}
