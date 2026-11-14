#include "Game.h"

// Entry point. Kept tiny on purpose: all the real work lives in Game.
int main()
{
    Game game;   // Constructor opens the window (no `new` needed in C++)
    game.run();  // Runs the game loop until the window closes
    return 0;    // Destructor closes the window automatically when `game` goes out of scope
}
