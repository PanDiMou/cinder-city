// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "log.hpp"

#include <SDL3/SDL.h>

namespace cinder::log::detail {
    void write(const level level, const std::string &message) {
        switch (level) {
            case level::info:
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
                break;
            case level::warning:
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
                break;
            case level::error:
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
                break;
        }
    }
}
