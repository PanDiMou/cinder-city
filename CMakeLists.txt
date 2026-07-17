cmake_minimum_required(VERSION 3.28)
project(CinderCity LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.4.12
        GIT_SHALLOW    ON
)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
        GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(glm)
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
set(SDLSHADERCROSS_CLI      OFF CACHE BOOL "" FORCE)  # pas d'outil ligne de commande
FetchContent_MakeAvailable(SDL3_shadercross)

add_executable(CinderCity
        src/main.cpp
        src/engine/core/window.cpp
        src/engine/core/application.cpp
        src/engine/core/platform.cpp
        src/engine/render/graphics_device.cpp
        src/engine/core/log.cpp
        src/engine/world/ground.cpp
        src/engine/world/cube.cpp
        src/engine/world/player.cpp
        src/engine/render/gpu_buffer.cpp
        src/engine/render/shader.cpp
        src/engine/render/renderer.cpp
        src/engine/render/gpu_mesh.cpp
)
target_include_directories(CinderCity PRIVATE src)
target_link_libraries(CinderCity PRIVATE SDL3::SDL3 glm::glm SDL3_shadercross::SDL3_shadercross)
target_compile_definitions(CinderCity PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)
