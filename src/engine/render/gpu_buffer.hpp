// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_GPU_BUFFER_HPP
#define CINDER_CITY_GPU_BUFFER_HPP

#include <cstdint>

struct SDL_GPUDevice;
struct SDL_GPUBuffer;

namespace cinder {
    class graphics_device;

    enum class buffer_usage {
        vertex,
        index
    };

    // A GPU buffer (vertex or index). Uploads the data on construction, releases it on destruction.
    class gpu_buffer {
    public:
        gpu_buffer(const graphics_device& device, const void* data, std::uint32_t size, buffer_usage usage);
        gpu_buffer(const gpu_buffer&) = delete;
        gpu_buffer& operator=(const gpu_buffer&) = delete;

        ~gpu_buffer();

        [[nodiscard]] SDL_GPUBuffer* native() const noexcept { return buffer_; }

    private:
        SDL_GPUDevice* device_ {nullptr};
        SDL_GPUBuffer* buffer_ {nullptr};
    };
}

#endif //CINDER_CITY_GPU_BUFFER_HPP
