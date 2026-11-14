#pragma once  // Prevents this header from being included twice in one file

#include <raylib.h>

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

    Player player_;               // Stored directly inside Game, not as a pointer
    Camera2D camera_{};           // `{}` zero-initializes every field
};
