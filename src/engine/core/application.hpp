// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_APPLICATION_HPP
#define CINDER_CITY_APPLICATION_HPP

#include "engine/core/platform.hpp"
#include "engine/core/window.hpp"
#include "engine/render/graphics_device.hpp"

namespace cinder {
    class application {
    public:
        application() = default;
        application(const application&) = delete;
        application& operator=(const application&) = delete;

        void run();
    private:
        void process_events();
        void render() const;

        platform        platform_;
        window          window_;
        graphics_device graphics_device_ {window_};
        bool            running_ {true};
    };
}

#endif //CINDER_CITY_APPLICATION_HPP
