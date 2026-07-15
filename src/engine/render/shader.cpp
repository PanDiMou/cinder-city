// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/render/shader.hpp"
#include "engine/render/graphics_device.hpp"

#include <SDL3/SDL.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace cinder {
    namespace {
        std::vector<std::uint8_t> read_file(const std::string& path) {
            std::ifstream file {path, std::ios::binary | std::ios::ate};
            if (!file) {
                throw std::runtime_error("Shader not found: " + path);
            }
            const auto size {static_cast<std::size_t>(file.tellg())};
            std::vector<std::uint8_t> bytes(size);
            file.seekg(0);
            file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(size));
            return bytes;
        }
    }

    shader::shader(const graphics_device& device, const std::string& spirv_path, const shader_stage stage)
        : device_ {device.native()} {

        const std::vector bytecode {read_file(spirv_path)};

        SDL_ShaderCross_SPIRV_Info info {};
        info.bytecode = bytecode.data();
        info.bytecode_size = bytecode.size();
        info.entrypoint = "main";
        info.shader_stage = stage == shader_stage::vertex
            ? SDL_SHADERCROSS_SHADERSTAGE_VERTEX
            : SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;

        // Reflection auto-detects the resources used (uniform buffers, samplers, ...).
        SDL_ShaderCross_GraphicsShaderMetadata* metadata {
            SDL_ShaderCross_ReflectGraphicsSPIRV(bytecode.data(), bytecode.size(), 0)
        };
        if (metadata == nullptr) {
            throw std::runtime_error("SDL_ShaderCross_ReflectGraphicsSPIRV: " + std::string{SDL_GetError()});
        }

        shader_ = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
            device_, &info, &metadata->resource_info, 0);
        SDL_free(metadata);

        if (shader_ == nullptr) {
            throw std::runtime_error("SDL_ShaderCross_CompileGraphicsShaderFromSPIRV: " + std::string{SDL_GetError()});
        }
    }

    shader::~shader() {
        if (shader_ != nullptr) {
            SDL_ReleaseGPUShader(device_, shader_);
        }
    }
}
