// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#ifndef CINDER_CITY_MODEL_CATALOG_HPP
#define CINDER_CITY_MODEL_CATALOG_HPP

#include <memory>
#include <string>
#include <unordered_map>

namespace cinder {
    class graphics_device;
    class gpu_mesh;

    // Loads each model once and reuses it for every instance, keyed by name.
    // A name maps to the file "assets/models/<name>.fbx".
    class model_catalog {
    public:
        explicit model_catalog(const graphics_device&);
        model_catalog(const model_catalog&) = delete;
        model_catalog& operator=(const model_catalog&) = delete;

        ~model_catalog();

        // Returns the GPU mesh for a model name, loading it on first request.
        [[nodiscard]] const gpu_mesh& get(const std::string&);

    private:
        const graphics_device* device_;
        std::unordered_map<std::string, std::unique_ptr<gpu_mesh>> meshes_;
    };
}

#endif //CINDER_CITY_MODEL_CATALOG_HPP
