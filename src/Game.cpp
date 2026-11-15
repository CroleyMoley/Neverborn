#include "Game.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include <raymath.h>

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

// Spawning: starts slow and speeds up the longer you survive.
constexpr float kStartSpawnInterval = 1.0f;   // Seconds between spawns at the start
constexpr float kMinSpawnInterval = 0.08f;    // Fastest it can get
constexpr float kSpawnIntervalDecay = 0.006f; // Interval shrinks by this much per second survived
constexpr std::size_t kMaxEnemies = 600;
constexpr float kSpawnMargin = 60.0f;         // How far outside the screen edge enemies appear

constexpr float kEnemyRadius = 12.0f;
constexpr float kEnemyMinSpeed = 70.0f;
constexpr float kEnemyMaxSpeed = 110.0f;
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
    elapsed_ += dt;

    player_.update(dt);

    updateSpawning(dt);

    // `Enemy&` (a reference) means "work on the actual enemy in the vector".
    // Without the &, each loop iteration would get a *copy*, and the movement would be lost.
    // (In Java, objects are always references, so this distinction doesn't exist there.)
    for (Enemy& enemy : enemies_)
    {
        enemy.update(dt, player_.position());
    }
    separateEnemies();

    // Keep the player centered on screen. `offset` is where on screen the
    // camera's `target` appears. Recomputed each frame in case the window was resized.
    camera_.target = player_.position();
    camera_.offset = Vector2{static_cast<float>(GetScreenWidth()) / 2.0f, static_cast<float>(GetScreenHeight()) / 2.0f};
}

void Game::updateSpawning(float dt)
{
    spawnTimer_ -= dt;
    if (spawnTimer_ > 0.0f || enemies_.size() >= kMaxEnemies)
    {
        return;
    }

    spawnEnemy();
    spawnTimer_ = std::max(kMinSpawnInterval, kStartSpawnInterval - elapsed_ * kSpawnIntervalDecay);
}

// Spawns one enemy at a random point on a circle just outside the visible screen,
// so enemies walk in from off-screen instead of popping into view.
void Game::spawnEnemy()
{
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> speedDist(kEnemyMinSpeed, kEnemyMaxSpeed);

    // Half the screen's diagonal = distance from the center (the player) to a corner.
    const float halfDiagonal = 0.5f * std::hypot(static_cast<float>(GetScreenWidth()),
                                                 static_cast<float>(GetScreenHeight()));
    const float distance = halfDiagonal + kSpawnMargin;

    const float angle = angleDist(rng_);
    const Vector2 offset{std::cos(angle) * distance, std::sin(angle) * distance};

    // emplace_back constructs the Enemy directly inside the vector (no temporary copy).
    enemies_.emplace_back(Vector2Add(player_.position(), offset), speedDist(rng_), kEnemyRadius);
}

// Pushes overlapping enemies apart so they form a crowd instead of one stacked blob.
//
// NOTE: This is the simple O(n^2) version: every enemy is compared with every other enemy.
// 300 enemies = ~45,000 checks per frame, 3,000 enemies = ~4.5 million. That's fine for now,
// and later we'll replace it with a spatial grid and measure the speedup. (Great resume bullet.)
void Game::separateEnemies()
{
    for (std::size_t i = 0; i < enemies_.size(); ++i)
    {
        for (std::size_t j = i + 1; j < enemies_.size(); ++j)
        {
            const Vector2 delta = Vector2Subtract(enemies_[j].position(), enemies_[i].position());
            const float minDistance = enemies_[i].radius() + enemies_[j].radius();
            const float distanceSq = Vector2LengthSqr(delta);

            // Comparing squared distances avoids a slow square root for the common "not touching" case.
            if (distanceSq >= minDistance * minDistance || distanceSq < 0.0001f)
            {
                continue;
            }

            const float distance = std::sqrt(distanceSq);
            const Vector2 normal = Vector2Scale(delta, 1.0f / distance);
            const float halfOverlap = 0.5f * (minDistance - distance);

            // Move each enemy half the overlap, in opposite directions.
            enemies_[i].push(Vector2Scale(normal, -halfOverlap));
            enemies_[j].push(Vector2Scale(normal, halfOverlap));
        }
    }
}

void Game::draw() const
{
    BeginDrawing();
    ClearBackground(kBackgroundColor);

    // Everything between BeginMode2D and EndMode2D is drawn in *world* coordinates,
    // and the camera converts it to screen pixels.
    BeginMode2D(camera_);
    drawGroundGrid();
    for (const Enemy& enemy : enemies_)  // const reference: read-only, no copy
    {
        enemy.draw();
    }
    player_.draw();
    EndMode2D();

    drawHud();

    EndDrawing();
}

// Drawn after EndMode2D, so it stays fixed on screen.
void Game::drawHud() const
{
    const int totalSeconds = static_cast<int>(elapsed_);

    DrawFPS(10, 10);
    DrawText(TextFormat("Time: %02d:%02d", totalSeconds / 60, totalSeconds % 60), 10, 36, 20, RAYWHITE);
    DrawText(TextFormat("Enemies: %d", static_cast<int>(enemies_.size())), 10, 60, 20, RAYWHITE);
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
