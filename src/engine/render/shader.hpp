// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_SHADER_HPP
#define CINDER_CITY_SHADER_HPP

#include <string>

struct SDL_GPUDevice;
struct SDL_GPUShader;

namespace cinder {
    class graphics_device;

    enum class shader_stage {
        vertex,
        fragment
    };

    // A GPU shader loaded from a SPIR-V file and translated to the device's native format.
    class shader {
    public:
        shader(const graphics_device&, const std::string&, shader_stage);
        shader(const shader&) = delete;
        shader& operator=(const shader&) = delete;

        ~shader();

        [[nodiscard]] SDL_GPUShader* native() const noexcept { return shader_; }

    private:
        SDL_GPUDevice* device_ {nullptr};
        SDL_GPUShader* shader_ {nullptr};
    };
}

#endif //CINDER_CITY_SHADER_HPP
