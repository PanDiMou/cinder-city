// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_PLATFORM_HPP
#define CINDER_CITY_PLATFORM_HPP

namespace cinder {
    class platform {
    public:
        platform();
        platform(const platform&) = delete;
        platform& operator=(const platform&) = delete;

        ~platform();
    };
}

#endif //CINDER_CITY_PLATFORM_HPP
