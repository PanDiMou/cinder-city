// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/core/application.hpp"
#include "engine/core/log.hpp"

#include <cstdlib>
#include <exception>

int main() {
    try {
        cinder::log::info("Cinder City — starting up");
        cinder::application app;
        app.run();
        return EXIT_SUCCESS;
    } catch (const std::exception& exception) {
        cinder::log::error("Fatal error: {}", exception.what());
    } catch (...) {
        cinder::log::error("Unknown fatal error");
    }

    return EXIT_FAILURE;
}
