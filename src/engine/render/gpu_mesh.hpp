// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_GPU_MESH_HPP
#define CINDER_CITY_GPU_MESH_HPP

#include "engine/render/gpu_buffer.hpp"

#include <cstdint>

struct SDL_GPURenderPass;

namespace cinder {
    class graphics_device;
    struct mesh;

    // GPU-side geometry (vertex + index buffers). Shared: many entities point to one.
    class gpu_mesh {
    public:
        gpu_mesh(const graphics_device&, const mesh&);
        gpu_mesh(const gpu_mesh&) = delete;
        gpu_mesh& operator=(const gpu_mesh&) = delete;

        void bind_and_draw(SDL_GPURenderPass*) const;
    private:
        gpu_buffer vertices_;
        gpu_buffer indices_;
        std::uint32_t index_count_ {0};
    };
}

#endif //CINDER_CITY_GPU_MESH_HPP
