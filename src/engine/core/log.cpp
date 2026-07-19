// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// Implémentation de la fonction interne write() déclarée dans log.hpp.
// Elle reçoit un message déjà formaté et le transmet au système de logs de SDL.

#include "log.hpp"

#include <SDL3/SDL.h>

namespace cinder::log::detail {
    void write(const level level, const std::string &message) {
        // Selon le niveau, on appelle la fonction SDL correspondante.
        // SDL colore/route les messages différemment (info, warn, error).
        // Le "%s" + message.c_str() : SDL utilise le style printf, et c_str()
        // convertit notre std::string en chaîne C (un simple const char*).
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
