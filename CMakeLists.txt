cmake_minimum_required(VERSION 3.24)
project(Neverborn LANGUAGES C CXX)

# Use modern C++ (C++20) and fail if the compiler can't do it.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Dependencies
# FetchContent downloads raylib the first time CMake configures the project,
# so nobody has to install it by hand. Pinning a version keeps builds repeatable.
# ---------------------------------------------------------------------------
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

# ---------------------------------------------------------------------------
# The game
# Add every new .cpp file to this list.
# ---------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Projectile.cpp
    src/Weapon.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Turn on strict compiler warnings. Treat them as hints about real bugs.
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
