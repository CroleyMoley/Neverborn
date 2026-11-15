# Neverborn

A top-down survival game in the "survivors-like" genre, written in modern C++ with [raylib](https://www.raylib.com/).
Survive endless waves of enemies with auto-firing weapons and level-up upgrades.

> Work in progress. A gameplay GIF, feature list and technical write-up are coming as the game develops.

## Controls

| Key | Action |
|---|---|
| WASD / Arrow keys | Move |
| R / Enter | Restart after game over |
| Esc | Quit |

## Building

Requirements: a C++20 compiler and CMake 3.24+. raylib is downloaded automatically.

**Windows (Visual Studio):** File → Open → Folder… → select this folder, choose `Neverborn.exe` as the startup item, press F5.

**Command line (any OS):**

```bash
cmake -S . -B build
cmake --build build --config Release
```

## Tech

- C++20, CMake, raylib 5.5