#pragma once

#include <raylib.h>

class Player
{
public:
    // `explicit` stops C++ from silently converting a Vector2 into a Player.
    explicit Player(Vector2 startPosition);

    void update(float dt);
    void draw() const;

    // Small getters are often defined right in the header.
    // `const` after the () means "calling this doesn't modify the Player".
    Vector2 position() const { return position_; }
    float radius() const { return radius_; }

private:
    // Trailing underscore = member variable. A common C++ naming convention.
    Vector2 position_;
    float speed_ = 220.0f;   // World units (pixels) per second
    float radius_ = 16.0f;
};
