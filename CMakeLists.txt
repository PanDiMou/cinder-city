cmake_minimum_required(VERSION 3.28)

# Le C est nécessaire en plus du C++ car ufbx est livré comme un unique fichier source C.
project(CinderCity LANGUAGES C CXX)

# Standard du langage : C++23 strict, sans extensions propres au compilateur.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Dépendances (récupérées et compilées depuis les sources, épinglées à des versions fixes).
# ---------------------------------------------------------------------------
include(FetchContent)

# SDL3 : fenêtrage, entrées et l'API de rendu GPU.
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.4.12
        GIT_SHALLOW    ON
)

# GLM : algèbre linéaire header-only (matrices, quaternions).
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
        GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(glm)

# Compile SDL3 uniquement en bibliothèque partagée, sans sa suite de tests.
set(SDL_SHARED ON  CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)

# SDL_shadercross : traduit le SPIR-V vers le format natif (Metal/MSL sur Mac) au runtime.
FetchContent_Declare(
        SDL3_shadercross
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_shadercross.git
        GIT_TAG        main
        GIT_SHALLOW    ON
)
set(SDLSHADERCROSS_SHARED   OFF CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_STATIC   ON  CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_VENDORED ON  CACHE BOOL "" FORCE)  # embarque SPIRV-Cross
set(SDLSHADERCROSS_DXC      OFF CACHE BOOL "" FORCE)  # pas de HLSL/DXC : on part du SPIR-V
set(SDLSHADERCROSS_CLI      OFF CACHE BOOL "" FORCE)  # pas d'outil en ligne de commande
FetchContent_MakeAvailable(SDL3_shadercross)

# ufbx : chargeur FBX en un seul fichier (sert à importer les modèles 3D).
FetchContent_Declare(
        ufbx
        GIT_REPOSITORY https://github.com/ufbx/ufbx.git
        GIT_TAG        v0.23.0
        GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(ufbx)

# stb : chargeur d'images header-only (sert à décoder les textures PNG).
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# nlohmann/json : parseur JSON header-only (sert aux fichiers de scène).
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
        GIT_SHALLOW    ON
)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nlohmann_json)

# Dear ImGui : UI en mode immédiat pour l'éditeur intégré.
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.92.8
        GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(imgui)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(CinderCity
        src/main.cpp
        src/engine/core/window.cpp
        src/engine/core/application.cpp
        src/engine/core/platform.cpp
        src/engine/core/log.cpp
        src/engine/render/graphics_device.cpp
        src/engine/render/gpu_buffer.cpp
        src/engine/render/gpu_mesh.cpp
        src/engine/render/shader.cpp
        src/engine/render/texture.cpp
        src/engine/render/renderer.cpp
        src/engine/world/ground.cpp
        src/engine/assets/fbx_loader.cpp
        src/engine/assets/stb_image_impl.cpp
        src/engine/assets/model_catalog.cpp
        src/engine/scene/scene_loader.cpp
        src/engine/editor/ui.cpp
)

# Inclut depuis la racine des sources pour que les headers utilisent des chemins complets "engine/...".
target_include_directories(CinderCity PRIVATE src)
target_link_libraries(CinderCity PRIVATE SDL3::SDL3 glm::glm SDL3_shadercross::SDL3_shadercross nlohmann_json::nlohmann_json)

# Aligne la convention de profondeur clip-space de GLM sur le backend GPU ([0, 1]).
target_compile_definitions(CinderCity PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)

# ufbx n'a pas de cible CMake : on compile son unique source directement dans la nôtre.
target_sources(CinderCity PRIVATE ${ufbx_SOURCE_DIR}/ufbx.c)
target_include_directories(CinderCity PRIVATE ${ufbx_SOURCE_DIR})

# stb est header-only : on expose ses headers (l'implémentation est dans stb_image_impl.cpp).
target_include_directories(CinderCity PRIVATE ${stb_SOURCE_DIR})

# Dear ImGui n'a pas de cible CMake : on compile son cœur + les backends SDL3/SDL_gpu dans la nôtre.
target_sources(CinderCity PRIVATE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
)
target_include_directories(CinderCity PRIVATE ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
