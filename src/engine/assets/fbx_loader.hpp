// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_FBX_LOADER_HPP
#define CINDER_CITY_FBX_LOADER_HPP

#include <string>

#include "engine/render/mesh.hpp"

namespace cinder {
    // Loads a static mesh from an FBX file, converted to meters and Y-up.
    // Every mesh node in the file is baked into a single geometry.
    // Throws std::runtime_error on failure.
    [[nodiscard]] mesh load_fbx(const std::string& path);
}

#endif //CINDER_CITY_FBX_LOADER_HPP
