#pragma once  // Prevents this header from being included twice in one file

#include <random>
#include <vector>

#include <raylib.h>

#include "Enemy.h"
#include "ExperienceGem.h"
#include "Player.h"
#include "Projectile.h"
#include "Weapon.h"

// `enum class` is a type-safe enum: you must write GameState::Playing, and it
// can't be accidentally compared with plain integers.
enum class GameState
{
    Playing,
    GameOver,
};

// Owns the window, the game loop, and everything in the world.
//
// Java note: C++ splits a class into a header (.h, the declaration: "what exists")
// and a source file (.cpp, the definition: "how it works"). Other files only
// #include the header.
class Game
{
public:
    Game();   // Opens the window
    ~Game();  // Destructor: closes the window. Runs automatically, like a guaranteed `finally`.

    // There is only one window, so copying a Game would be a bug. Delete the copy operations
    // so the compiler refuses to do it.
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void run();

private:
    void update(float dt);        // Game logic: movement, spawning, collisions...
    void draw() const;            // Rendering only. `const` = promises not to change the game state.
    void drawGroundGrid() const;
    void drawHud() const;
    void drawGameOver() const;

    void reset();                 // Starts a fresh run
    void updateSpawning(float dt);
    void spawnEnemy();
    void separateEnemies();
    void checkPlayerHits();
    void handleProjectileHits();
    void removeDeadEntities();
    void updateGems(float dt);
    void drawGems() const;
    void drawExperienceBar() const;

    GameState state_ = GameState::Playing;

    Player player_;               // Stored directly inside Game, not as a pointer
    Camera2D camera_{};           // `{}` zero-initializes every field

    // std::vector is C++'s ArrayList. It stores the Enemy objects themselves
    // (not pointers to them) side by side in memory, which is fast to loop over.
    std::vector<Enemy> enemies_;
    std::vector<Projectile> projectiles_;
    std::vector<ExperienceGem> gems_;

    // A single weapon for now. Later this becomes a list so the player can collect several.
    Weapon weapon_;

    // Random number generator, seeded from the OS. Preferred over the old C rand().
    std::mt19937 rng_{ std::random_device{}() };

    float spawnTimer_ = 0.0f;     // Seconds until the next enemy spawns
    float elapsed_ = 0.0f;        // Seconds survived this run
    int kills_ = 0;
    float levelUpBannerTimer_ = 0.0f;  // Shows "LEVEL UP!" while > 0
};
