// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_APPLICATION_HPP
#define CINDER_CITY_APPLICATION_HPP

#include "engine/core/platform.hpp"
#include "engine/core/window.hpp"
#include "engine/render/graphics_device.hpp"
#include "engine/render/renderer.hpp"
#include "engine/render/gpu_mesh.hpp"
#include "engine/scene/camera.hpp"
#include "engine/world/cube.hpp"
#include "engine/world/ground.hpp"
#include "engine/world/player.hpp"
#include "engine/world/world.hpp"

namespace cinder {
    class application {
    public:
        application();
        application(const application&) = delete;
        application& operator=(const application&) = delete;

        void run();
    private:
        void process_events();
        void render() const;

        platform        platform_;
        window          window_;
        graphics_device graphics_device_ {window_};
        renderer        renderer_ {graphics_device_};
        ground          ground_;
        gpu_mesh        ground_mesh_ {graphics_device_, ground_.geometry()};
        cube            cube_;
        gpu_mesh        cube_mesh_ {graphics_device_, cube_.geometry()};
        world           world_;
        camera          camera_;
        player*         player_ {nullptr};   // owned by world_, kept for the follow camera
        bool            running_ {true};
    };
}

#endif //CINDER_CITY_APPLICATION_HPP
