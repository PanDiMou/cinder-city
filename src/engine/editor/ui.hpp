// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_UI_HPP
#define CINDER_CITY_UI_HPP

struct SDL_GPUDevice;
struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;
union SDL_Event;

namespace cinder {
    class graphics_device;

    // Owns the Dear ImGui context and its SDL3 / SDL_gpu backends.
    // Per frame: process_event() -> begin_frame() -> (build widgets) -> finalize()
    // -> upload() -> render(). RAII: sets up on construction, tears down on destruction.
    class ui {
    public:
        explicit ui(const graphics_device& device);
        ui(const ui&) = delete;
        ui& operator=(const ui&) = delete;

        ~ui();

        void process_event(const SDL_Event& event);  // feed an SDL event to ImGui
        void begin_frame();                           // start a new ImGui frame
        void finalize();                              // close the frame (ImGui::Render)
        void upload(SDL_GPUCommandBuffer* command_buffer);                       // stage draw data
        void render(SDL_GPUCommandBuffer* command_buffer, SDL_GPURenderPass* pass); // draw it

    private:
        SDL_GPUDevice* device_ {nullptr};
    };
}

#endif //CINDER_CITY_UI_HPP
