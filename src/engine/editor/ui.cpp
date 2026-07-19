// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/editor/ui.hpp"
#include "engine/render/graphics_device.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

#include <SDL3/SDL.h>

namespace cinder {
    ui::ui(const graphics_device& device)
        : device_ {device.native()} {

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLGPU(device.window_handle());

        ImGui_ImplSDLGPU3_InitInfo init {};
        init.Device = device_;
        init.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(device_, device.window_handle());
        init.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
        ImGui_ImplSDLGPU3_Init(&init);
    }

    ui::~ui() {
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void ui::process_event(const SDL_Event& event) {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    void ui::begin_frame() {
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void ui::finalize() {
        ImGui::Render();
    }

    void ui::upload(SDL_GPUCommandBuffer* command_buffer) {
        // Stage the frame's vertex/index data onto the command buffer (before any render pass).
        ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), command_buffer);
    }

    void ui::render(SDL_GPUCommandBuffer* command_buffer, SDL_GPURenderPass* pass) {
        ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), command_buffer, pass);
    }
}
