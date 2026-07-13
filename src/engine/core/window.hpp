// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_WINDOW_HPP
#define CINDER_CITY_WINDOW_HPP

struct SDL_Window;

namespace cinder {
    class window {
    public:
        window();
        window(const window&) = delete;
        window& operator=(const window&) = delete;

        ~window();
    private:
        SDL_Window* handle_ {nullptr};
    };
}

#endif //CINDER_CITY_WINDOW_HPP
