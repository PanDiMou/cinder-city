// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/assets/model_catalog.hpp"
#include "engine/assets/fbx_loader.hpp"
#include "engine/render/gpu_mesh.hpp"

#include <utility>

namespace cinder {
    model_catalog::model_catalog(const graphics_device& device)
        : device_ {&device} {}

    // Defined here (not in the header) so unique_ptr sees the complete gpu_mesh type.
    model_catalog::~model_catalog() = default;

    const gpu_mesh& model_catalog::get(const std::string& name) {
        // Already loaded: hand back the shared mesh.
        if (const auto found {meshes_.find(name)}; found != meshes_.end()) {
            return *found->second;
        }

        // First request: load the FBX once, upload it, and cache it.
        // Take the reference from the map (not the local) so its lifetime is the cache's.
        const std::string path {"assets/models/" + name + ".fbx"};
        const auto [entry, inserted] {meshes_.emplace(name, std::make_unique<gpu_mesh>(*device_, load_fbx(path)))};
        return *entry->second;
    }
}
