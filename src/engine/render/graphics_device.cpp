// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/render/graphics_device.hpp"
#include "engine/core/window.hpp"

#include <SDL3/SDL.h>
#include <SDL3_shadercross/SDL_shadercross.h>
#include <stdexcept>
#include <string>

#ifdef NDEBUG
    constexpr bool debug_mode {false};
#else
    constexpr bool debug_mode {true};
#endif

namespace cinder {
    graphics_device::graphics_device(window& window): window_ {window.native()} {
        device_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV
            | SDL_GPU_SHADERFORMAT_MSL
            | SDL_GPU_SHADERFORMAT_DXIL,debug_mode, nullptr);

        if (device_ == nullptr) {
            throw std::runtime_error(std::string{"SDL_CreateGPUDevice: "} + SDL_GetError());
        }

        if (!SDL_ClaimWindowForGPUDevice(device_, window_)) {
            SDL_DestroyGPUDevice(device_);
            throw std::runtime_error(std::string{"SDL_ClaimWindowForGPUDevice: "} + SDL_GetError());
        }

        if (!SDL_ShaderCross_Init()) {
            SDL_ReleaseWindowFromGPUDevice(device_, window_);
            SDL_DestroyGPUDevice(device_);
            throw std::runtime_error(std::string{"SDL_ShaderCross_Init: "} + SDL_GetError());
        }
    }

    graphics_device::~graphics_device() {
        SDL_ShaderCross_Quit();
        if (device_ != nullptr) {
            SDL_ReleaseWindowFromGPUDevice(device_, window_);
            SDL_DestroyGPUDevice(device_);
        }
    }
}
