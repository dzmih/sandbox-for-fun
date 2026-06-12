cmake_minimum_required(VERSION 3.20)

project(FallingSandGame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

add_executable(falling_sand
    src/main.cpp
    src/world.cpp
    src/ui.cpp
)

target_link_libraries(falling_sand PRIVATE raylib)

if (MSVC)
    target_compile_options(falling_sand PRIVATE /W4 /permissive-)
else()
    target_compile_options(falling_sand PRIVATE -Wall -Wextra -Wpedantic)
endif()
