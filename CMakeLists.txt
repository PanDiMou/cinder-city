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

add_executable(CinderCity
        src/main.cpp
        src/engine/core/window.cpp
        src/engine/core/application.cpp
        src/engine/core/platform.cpp
        src/engine/render/graphics_device.cpp
        src/engine/core/log.cpp
        src/engine/world/ground.cpp
)
target_include_directories(CinderCity PRIVATE src)
target_link_libraries(CinderCity PRIVATE SDL3::SDL3 glm::glm)

