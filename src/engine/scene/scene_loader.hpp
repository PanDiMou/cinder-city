// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_SCENE_LOADER_HPP
#define CINDER_CITY_SCENE_LOADER_HPP

#include <string>

namespace cinder {
    class world;
    class model_catalog;

    // Reads a JSON scene file and spawns its props into the world, resolving each
    // model through the catalog. Throws std::runtime_error on failure.
    void load_scene(const std::string&, world&, model_catalog&);
}

#endif //CINDER_CITY_SCENE_LOADER_HPP
