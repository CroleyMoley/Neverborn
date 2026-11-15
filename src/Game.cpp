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
    constexpr Color kBackgroundColor{ 24, 20, 37, 255 };
    constexpr Color kGridColor{ 44, 38, 64, 255 };

    // Spawning: starts slow and speeds up the longer you survive.
    constexpr float kStartSpawnInterval = 1.0f;   // Seconds between spawns at the start
    constexpr float kMinSpawnInterval = 0.08f;    // Fastest it can get
    constexpr float kSpawnIntervalDecay = 0.006f; // Interval shrinks by this much per second survived
    constexpr std::size_t kMaxEnemies = 600;
    constexpr float kSpawnMargin = 60.0f;         // How far outside the screen edge enemies appear

    constexpr float kEnemyRadius = 12.0f;
    constexpr float kEnemyMinSpeed = 70.0f;
    constexpr float kEnemyMaxSpeed = 110.0f;
    constexpr float kEnemyContactDamage = 10.0f;
    constexpr float kEnemyHealth = 20.0f;         // 2 hits from the starting weapon
    constexpr float kKnockbackDistance = 6.0f;    // How far a hit pushes an enemy back
}  // namespace

// The `: player_(...)` part is a member initializer list. It constructs members
// before the constructor body runs.
Game::Game()
    : player_(Vector2{ 0.0f, 0.0f })
    , weapon_(WeaponStats{})
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
    if (state_ == GameState::GameOver)
    {
        // The world stays frozen until the player restarts.
        if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER))
        {
            reset();
        }
        return;
    }

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

    weapon_.update(dt, player_.position(), enemies_, projectiles_);
    for (Projectile& projectile : projectiles_)
    {
        projectile.update(dt);
    }
    handleProjectileHits();
    removeDeadEntities();

    checkPlayerHits();

    // Keep the player centered on screen. `offset` is where on screen the
    // camera's `target` appears. Recomputed each frame in case the window was resized.
    camera_.target = player_.position();
    camera_.offset = Vector2{ static_cast<float>(GetScreenWidth()) / 2.0f, static_cast<float>(GetScreenHeight()) / 2.0f };
}

void Game::reset()
{
    player_ = Player(Vector2{ 0.0f, 0.0f });
    enemies_.clear();
    projectiles_.clear();
    weapon_ = Weapon(WeaponStats{});
    kills_ = 0;
    spawnTimer_ = 0.0f;
    elapsed_ = 0.0f;
    state_ = GameState::Playing;
}

// Checks every live projectile against every enemy.
// Like separateEnemies(), this is O(projectiles x enemies). The spatial grid will speed this up too.
void Game::handleProjectileHits()
{
    for (Projectile& projectile : projectiles_)
    {
        if (projectile.isExpired())
        {
            continue;
        }

        for (Enemy& enemy : enemies_)
        {
            if (enemy.isDead())
            {
                continue;
            }

            if (CheckCollisionCircles(projectile.position(), projectile.radius(),
                enemy.position(), enemy.radius()))
            {
                enemy.takeDamage(projectile.damage());
                enemy.push(Vector2Scale(projectile.direction(), kKnockbackDistance));
                projectile.markHit();
                break;  // Each projectile hits only one enemy
            }
        }
    }
}

// Deletes dead enemies and used-up projectiles.
//
// std::erase_if (C++20) removes every element the lambda returns true for and
// returns how many were removed. Never remove items from a vector while looping
// over it with a range-for. That invalidates the loop and crashes.
void Game::removeDeadEntities()
{
    const auto enemiesRemoved = std::erase_if(enemies_, [](const Enemy& enemy) { return enemy.isDead(); });
    kills_ += static_cast<int>(enemiesRemoved);

    std::erase_if(projectiles_, [](const Projectile& projectile) { return projectile.isExpired(); });
}

// Circle-vs-circle collision between the player and every enemy.
// One check per enemy (O(n)) is cheap, unlike enemy-vs-enemy separation.
void Game::checkPlayerHits()
{
    for (const Enemy& enemy : enemies_)
    {
        if (CheckCollisionCircles(player_.position(), player_.radius(), enemy.position(), enemy.radius()))
        {
            player_.takeDamage(kEnemyContactDamage);
            break;  // One hit per frame is enough; i-frames block the rest anyway.
        }
    }

    if (player_.isDead())
    {
        state_ = GameState::GameOver;
    }
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
    const Vector2 offset{ std::cos(angle) * distance, std::sin(angle) * distance };

    // emplace_back constructs the Enemy directly inside the vector (no temporary copy).
    enemies_.emplace_back(Vector2Add(player_.position(), offset), speedDist(rng_), kEnemyRadius, kEnemyHealth);
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
    for (const Projectile& projectile : projectiles_)
    {
        projectile.draw();
    }
    player_.draw();
    EndMode2D();

    drawHud();
    if (state_ == GameState::GameOver)
    {
        drawGameOver();
    }

    EndDrawing();
}

void Game::drawGameOver() const
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    const int totalSeconds = static_cast<int>(elapsed_);

    // Darken everything behind the text.
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.6f));

    // A small lambda (an inline function, like a Java lambda) to center a line of text.
    auto drawCentered = [screenWidth](const char* text, int y, int fontSize, Color color) {
        DrawText(text, (screenWidth - MeasureText(text, fontSize)) / 2, y, fontSize, color);
        };

    const int centerY = screenHeight / 2;
    drawCentered("GAME OVER", centerY - 70, 60, Color{ 230, 70, 90, 255 });
    drawCentered(TextFormat("You survived %02d:%02d  |  %d kills", totalSeconds / 60, totalSeconds % 60, kills_),
        centerY + 5, 28, RAYWHITE);
    drawCentered("Press R to try again", centerY + 50, 22, Color{ 180, 180, 200, 255 });
}

// Drawn after EndMode2D, so it stays fixed on screen.
void Game::drawHud() const
{
    const int totalSeconds = static_cast<int>(elapsed_);

    DrawFPS(10, 10);
    DrawText(TextFormat("Time: %02d:%02d", totalSeconds / 60, totalSeconds % 60), 10, 36, 20, RAYWHITE);
    DrawText(TextFormat("Enemies: %d", static_cast<int>(enemies_.size())), 10, 60, 20, RAYWHITE);
    DrawText(TextFormat("Kills: %d", kills_), 10, 84, 20, RAYWHITE);
}

// Draws an endless grid so you can see the player moving.
// Only the lines currently on screen are drawn. Skipping off-screen work like this
// is called "culling", and it will matter a lot once we have thousands of enemies.
void Game::drawGroundGrid() const
{
    const Vector2 topLeft = GetScreenToWorld2D(Vector2{ 0.0f, 0.0f }, camera_);
    const Vector2 bottomRight = GetScreenToWorld2D(
        Vector2{ static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
        camera_);

    // Snap the first line to a multiple of the spacing so the grid doesn't slide with the camera.
    const float startX = std::floor(topLeft.x / kGridSpacing) * kGridSpacing;
    const float startY = std::floor(topLeft.y / kGridSpacing) * kGridSpacing;

    for (float x = startX; x <= bottomRight.x; x += kGridSpacing)
    {
        DrawLineV(Vector2{ x, topLeft.y }, Vector2{ x, bottomRight.y }, kGridColor);
    }
    for (float y = startY; y <= bottomRight.y; y += kGridSpacing)
    {
        DrawLineV(Vector2{ topLeft.x, y }, Vector2{ bottomRight.x, y }, kGridColor);
    }
}
