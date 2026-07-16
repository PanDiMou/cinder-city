// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "gpu_mesh.hpp"
#include "engine/render/mesh.hpp"

#include <SDL3/SDL.h>

namespace cinder {
    gpu_mesh::gpu_mesh(const graphics_device& device, const mesh& mesh):
        vertices_ {device, mesh.vertices.data(), static_cast<std::uint32_t>(mesh.vertices.size() * sizeof(vertex)), buffer_usage::vertex},
        indices_ {device, mesh.indices.data(), static_cast<std::uint32_t>(mesh.indices.size() * sizeof(std::uint32_t)), buffer_usage::index},
        index_count_ {static_cast<std::uint32_t>(mesh.indices.size())} { }

    void gpu_mesh::bind_and_draw(SDL_GPURenderPass* pass) const {
        const SDL_GPUBufferBinding vertex_binding {.buffer = vertices_.native(), .offset = 0};
        SDL_BindGPUVertexBuffers(pass, 0, &vertex_binding, 1);
        const SDL_GPUBufferBinding index_binding {.buffer = indices_.native(), .offset = 0};
        SDL_BindGPUIndexBuffer(pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
        SDL_DrawGPUIndexedPrimitives(pass, index_count_, 1, 0, 0, 0);
    }
}
