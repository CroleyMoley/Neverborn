#pragma once  // Prevents this header from being included twice in one file

#include <random>
#include <vector>

#include <raylib.h>

#include "Enemy.h"
#include "Player.h"

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

    void updateSpawning(float dt);
    void spawnEnemy();
    void separateEnemies();

    Player player_;               // Stored directly inside Game, not as a pointer
    Camera2D camera_{};           // `{}` zero-initializes every field

    // std::vector is C++'s ArrayList. It stores the Enemy objects themselves
    // (not pointers to them) side by side in memory, which is fast to loop over.
    std::vector<Enemy> enemies_;

    // Random number generator, seeded from the OS. Preferred over the old C rand().
    std::mt19937 rng_{ std::random_device{}() };

    float spawnTimer_ = 0.0f;     // Seconds until the next enemy spawns
    float elapsed_ = 0.0f;        // Seconds survived this run
};
