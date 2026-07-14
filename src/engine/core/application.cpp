// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "application.hpp"
#include "log.hpp"

#include <SDL3/SDL.h>

namespace cinder {
    void application::run() {
        while (running_) {
            process_events();
            render();
        }
    }

    void application::process_events() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT
                || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
                running_ = false;
            }
        }
    }

    void application::render() const {
        SDL_GPUCommandBuffer* command_buffer {SDL_AcquireGPUCommandBuffer(graphics_device_.native())};
        if (command_buffer == nullptr) {
            log::error("SDL_AcquireGPUCommandBuffer: {}", SDL_GetError());
            return;
        }

        SDL_GPUTexture* swapchain {nullptr};
        Uint32 width {0};
        Uint32 height {0};
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, graphics_device_.window_handle(),&swapchain, &width, &height) || swapchain == nullptr) {
            log::warning("SDL_WaitAndAcquireGPUSwapchainTexture: {}", SDL_GetError());
            SDL_SubmitGPUCommandBuffer(command_buffer); // submit even if empty, otherwise it leaks
            return;
        }

        SDL_GPUColorTargetInfo color {};
        color.texture = swapchain;
        color.clear_color = SDL_FColor {0.53f, 0.81f, 0.92f, 1.0f}; // sky blue
        color.load_op = SDL_GPU_LOADOP_CLEAR;
        color.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* pass {SDL_BeginGPURenderPass(command_buffer, &color, 1, nullptr)};
        SDL_EndGPURenderPass(pass);

        SDL_SubmitGPUCommandBuffer(command_buffer);
    }
}
