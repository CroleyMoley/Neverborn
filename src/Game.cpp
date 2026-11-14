#include "Game.h"

#include <algorithm>
#include <cmath>

// An anonymous namespace makes these constants private to this file
// (similar to `private static final` in Java).
namespace
{
constexpr int kScreenWidth = 1280;
constexpr int kScreenHeight = 720;
constexpr int kTargetFps = 60;

// If the game freezes for a moment (e.g. you drag the window), the next frame's
// delta time can be huge and the player would teleport. Cap it.
constexpr float kMaxFrameTime = 0.05f;

constexpr float kGridSpacing = 64.0f;
constexpr Color kBackgroundColor{24, 20, 37, 255};
constexpr Color kGridColor{44, 38, 64, 255};
}  // namespace

// The `: player_(...)` part is a member initializer list. It constructs members
// before the constructor body runs.
Game::Game()
    : player_(Vector2{0.0f, 0.0f})
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(kScreenWidth, kScreenHeight, "Neverborn");
    SetTargetFPS(kTargetFps);

    camera_.zoom = 1.0f;
}

Game::~Game()
{
    CloseWindow();
}

// The classic game loop: read input + update the world, then draw it, ~60 times per second.
void Game::run()
{
    while (!WindowShouldClose())  // True when the user presses Esc or closes the window
    {
        // dt ("delta time") = seconds since the last frame. Multiplying speeds by dt
        // makes the game run at the same speed on fast and slow computers.
        const float dt = std::min(GetFrameTime(), kMaxFrameTime);

        update(dt);
        draw();
    }
}

void Game::update(float dt)
{
    player_.update(dt);

    // Keep the player centered on screen. `offset` is where on screen the
    // camera's `target` appears. Recomputed each frame in case the window was resized.
    camera_.target = player_.position();
    camera_.offset = Vector2{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

void Game::draw() const
{
    BeginDrawing();
    ClearBackground(kBackgroundColor);

    // Everything between BeginMode2D and EndMode2D is drawn in *world* coordinates,
    // and the camera converts it to screen pixels.
    BeginMode2D(camera_);
    drawGroundGrid();
    player_.draw();
    EndMode2D();

    // Drawn after EndMode2D, so these stay fixed on screen (the HUD).
    DrawFPS(10, 10);
    DrawText("WASD / arrow keys to move", 10, 36, 20, RAYWHITE);

    EndDrawing();
}

// Draws an endless grid so you can see the player moving.
// Only the lines currently on screen are drawn. Skipping off-screen work like this
// is called "culling", and it will matter a lot once we have thousands of enemies.
void Game::drawGroundGrid() const
{
    const Vector2 topLeft = GetScreenToWorld2D(Vector2{0.0f, 0.0f}, camera_);
    const Vector2 bottomRight = GetScreenToWorld2D(
        Vector2{static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())},
        camera_);

    // Snap the first line to a multiple of the spacing so the grid doesn't slide with the camera.
    const float startX = std::floor(topLeft.x / kGridSpacing) * kGridSpacing;
    const float startY = std::floor(topLeft.y / kGridSpacing) * kGridSpacing;

    for (float x = startX; x <= bottomRight.x; x += kGridSpacing)
    {
        DrawLineV(Vector2{x, topLeft.y}, Vector2{x, bottomRight.y}, kGridColor);
    }
    for (float y = startY; y <= bottomRight.y; y += kGridSpacing)
    {
        DrawLineV(Vector2{topLeft.x, y}, Vector2{bottomRight.x, y}, kGridColor);
    }
}
