// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/render/renderer.hpp"
#include "engine/render/graphics_device.hpp"
#include "engine/render/shader.hpp"
#include "engine/render/mesh.hpp"
#include "engine/scene/camera.hpp"
#include "engine/core/log.hpp"

#include <SDL3/SDL.h>

#include <cstddef>
#include <stdexcept>
#include <string>

namespace cinder {
    namespace {
        constexpr auto depth_format {SDL_GPU_TEXTUREFORMAT_D32_FLOAT};
    }

    renderer::renderer(const graphics_device& device, const mesh& mesh)
        : device_ {device.native()},
          window_ {device.window_handle()},
          vertices_{
              device, mesh.vertices.data(), static_cast<std::uint32_t>(mesh.vertices.size() * sizeof(vertex)),
              buffer_usage::vertex
          },
          indices_{
              device, mesh.indices.data(), static_cast<std::uint32_t>(mesh.indices.size() * sizeof(std::uint32_t)),
              buffer_usage::index
          },
          index_count_ {static_cast<std::uint32_t>(mesh.indices.size())} {

        const shader vertex_shader {device, "shaders/ground.vert.spv", shader_stage::vertex};
        const shader fragment_shader {device, "shaders/ground.frag.spv", shader_stage::fragment};

        // How to read a vertex buffer: one buffer, one attribute (position at location 0).
        constexpr SDL_GPUVertexBufferDescription vertex_buffer {
            .slot = 0,
            .pitch = sizeof(vertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0
        };
        constexpr SDL_GPUVertexAttribute position_attribute {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(vertex, position)
        };

        const SDL_GPUColorTargetDescription color_target {
            .format = SDL_GetGPUSwapchainTextureFormat(device_, window_)
        };

        SDL_GPUGraphicsPipelineCreateInfo pipeline_info {};
        pipeline_info.vertex_shader = vertex_shader.native();
        pipeline_info.fragment_shader = fragment_shader.native();
        pipeline_info.vertex_input_state.vertex_buffer_descriptions = &vertex_buffer;
        pipeline_info.vertex_input_state.num_vertex_buffers = 1;
        pipeline_info.vertex_input_state.vertex_attributes = &position_attribute;
        pipeline_info.vertex_input_state.num_vertex_attributes = 1;
        pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        pipeline_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
        pipeline_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
        pipeline_info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
        pipeline_info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;
        pipeline_info.depth_stencil_state.enable_depth_test = true;
        pipeline_info.depth_stencil_state.enable_depth_write = true;
        pipeline_info.target_info.color_target_descriptions = &color_target;
        pipeline_info.target_info.num_color_targets = 1;
        pipeline_info.target_info.depth_stencil_format = depth_format;
        pipeline_info.target_info.has_depth_stencil_target = true;

        pipeline_ = SDL_CreateGPUGraphicsPipeline(device_, &pipeline_info);
        if (pipeline_ == nullptr) {
            throw std::runtime_error(std::string{"SDL_CreateGPUGraphicsPipeline: "} + SDL_GetError());
        }

        // Depth buffer, sized to the window (fullscreen: fixed size for now).
        int width {0};
        int height {0};
        SDL_GetWindowSizeInPixels(window_, &width, &height);

        SDL_GPUTextureCreateInfo depth_info {};
        depth_info.type = SDL_GPU_TEXTURETYPE_2D;
        depth_info.format = depth_format;
        depth_info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
        depth_info.width = static_cast<Uint32>(width);
        depth_info.height = static_cast<Uint32>(height);
        depth_info.layer_count_or_depth = 1;
        depth_info.num_levels = 1;
        depth_info.sample_count = SDL_GPU_SAMPLECOUNT_1;

        depth_ = SDL_CreateGPUTexture(device_, &depth_info);
        if (depth_ == nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, pipeline_);
            throw std::runtime_error(std::string{"SDL_CreateGPUTexture (depth): "} + SDL_GetError());
        }
    }

    renderer::~renderer() {
        if (depth_ != nullptr) {
            SDL_ReleaseGPUTexture(device_, depth_);
        }
        if (pipeline_ != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(device_, pipeline_);
        }
    }

    void renderer::draw(const camera& camera) const {
        SDL_GPUCommandBuffer* command_buffer {SDL_AcquireGPUCommandBuffer(device_)};
        if (command_buffer == nullptr) {
            log::error("SDL_AcquireGPUCommandBuffer: {}", SDL_GetError());
            return;
        }

        SDL_GPUTexture* swapchain {nullptr};
        Uint32 width {0};
        Uint32 height {0};
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, window_, &swapchain, &width, &height)) {
            log::warning("SDL_WaitAndAcquireGPUSwapchainTexture: {}", SDL_GetError());
            SDL_SubmitGPUCommandBuffer(command_buffer);
            return;
        }
        if (swapchain == nullptr) {
            SDL_SubmitGPUCommandBuffer(command_buffer); // window minimized
            return;
        }

        const float aspect {static_cast<float>(width) / static_cast<float>(height)};
        const glm::mat4 view_projection {camera.view_projection(aspect)};

        SDL_GPUColorTargetInfo color {};
        color.texture = swapchain;
        color.clear_color = SDL_FColor {0.53f, 0.81f, 0.92f, 1.0f}; // sky blue
        color.load_op = SDL_GPU_LOADOP_CLEAR;
        color.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depth {};
        depth.texture = depth_;
        depth.clear_depth = 1.0f;
        depth.load_op = SDL_GPU_LOADOP_CLEAR;
        depth.store_op = SDL_GPU_STOREOP_DONT_CARE;

        SDL_GPURenderPass* pass {SDL_BeginGPURenderPass(command_buffer, &color, 1, &depth)};

        SDL_BindGPUGraphicsPipeline(pass, pipeline_);

        const SDL_GPUBufferBinding vertex_binding {.buffer = vertices_.native(), .offset = 0};
        SDL_BindGPUVertexBuffers(pass, 0, &vertex_binding, 1);
        const SDL_GPUBufferBinding index_binding {.buffer = indices_.native(), .offset = 0};
        SDL_BindGPUIndexBuffer(pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        SDL_PushGPUVertexUniformData(command_buffer, 0, &view_projection, sizeof(view_projection));
        SDL_DrawGPUIndexedPrimitives(pass, index_count_, 1, 0, 0, 0);

        SDL_EndGPURenderPass(pass);
        SDL_SubmitGPUCommandBuffer(command_buffer);
    }
}
