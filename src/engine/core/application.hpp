// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_APPLICATION_HPP
#define CINDER_CITY_APPLICATION_HPP

#include "window.hpp"

#include <optional>

namespace cinder {
    class application {
    public:
        application();
        application(const application&) = delete;
        application& operator=(const application&) = delete;

        ~application();

        void run();
    private:
        void process_events();

        std::optional<window> window_;
        bool running_ {true};
    };
}

#endif //CINDER_CITY_APPLICATION_HPP
