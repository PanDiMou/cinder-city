cmake_minimum_required(VERSION 3.28)

# C is needed alongside C++ because ufbx ships as a single C source file.
project(CinderCity LANGUAGES C CXX)

# Language standard: strict C++23, no compiler-specific extensions.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Dependencies (fetched and built from source, pinned to fixed versions).
# ---------------------------------------------------------------------------
include(FetchContent)

# SDL3: windowing, input and the GPU rendering API.
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.4.12
        GIT_SHALLOW    ON
)

# GLM: header-only linear algebra (matrices, quaternions).
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
        GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(glm)

# Build SDL3 as a shared library only, without its test suite.
set(SDL_SHARED ON  CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)

# SDL_shadercross: translates SPIR-V to the native format (Metal/MSL on Mac) at runtime.
FetchContent_Declare(
        SDL3_shadercross
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_shadercross.git
        GIT_TAG        main
        GIT_SHALLOW    ON
)
set(SDLSHADERCROSS_SHARED   OFF CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_STATIC   ON  CACHE BOOL "" FORCE)
set(SDLSHADERCROSS_VENDORED ON  CACHE BOOL "" FORCE)  # bundle SPIRV-Cross
set(SDLSHADERCROSS_DXC      OFF CACHE BOOL "" FORCE)  # no HLSL/DXC: we start from SPIR-V
set(SDLSHADERCROSS_CLI      OFF CACHE BOOL "" FORCE)  # no command-line tool
FetchContent_MakeAvailable(SDL3_shadercross)

# ufbx: single-source FBX loader (used to import 3D models).
FetchContent_Declare(
        ufbx
        GIT_REPOSITORY https://github.com/ufbx/ufbx.git
        GIT_TAG        v0.23.0
        GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(ufbx)

# stb: single-header image loader (used to decode PNG textures).
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# nlohmann/json: header-only JSON parser (used for scene files).
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
        GIT_SHALLOW    ON
)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nlohmann_json)

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
)

# Include from the source root so headers use full "engine/..." paths.
target_include_directories(CinderCity PRIVATE src)
target_link_libraries(CinderCity PRIVATE SDL3::SDL3 glm::glm SDL3_shadercross::SDL3_shadercross nlohmann_json::nlohmann_json)

# Match GLM's clip-space depth convention to the GPU backend ([0, 1]).
target_compile_definitions(CinderCity PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)

# ufbx has no CMake target: compile its single source into ours directly.
target_sources(CinderCity PRIVATE ${ufbx_SOURCE_DIR}/ufbx.c)
target_include_directories(CinderCity PRIVATE ${ufbx_SOURCE_DIR})

# stb is header-only: expose its headers (the implementation lives in stb_image_impl.cpp).
target_include_directories(CinderCity PRIVATE ${stb_SOURCE_DIR})
