#pragma once

#include <raylib.h>

class Player
{
public:
    // `explicit` stops C++ from silently converting a Vector2 into a Player.
    explicit Player(Vector2 startPosition);

    void update(float dt);
    void draw() const;

    // Applies damage unless the player is still invulnerable from a recent hit.
    // Returns true if the damage was actually applied.
    bool takeDamage(float amount);

    // Small getters are often defined right in the header.
    // `const` after the () means "calling this doesn't modify the Player".
    Vector2 position() const { return position_; }
    float radius() const { return radius_; }
    float health() const { return health_; }
    float maxHealth() const { return maxHealth_; }
    bool isDead() const { return health_ <= 0.0f; }

    // Adds XP and returns how many levels were gained (a big gem could give more than one).
    int addExperience(int amount);

    int level() const { return level_; }
    int experience() const { return experience_; }
    int experienceToNextLevel() const { return experienceToNextLevel_; }
    float pickupRadius() const { return pickupRadius_; }

private:
    void drawHealthBar() const;

    // Trailing underscore = member variable. A common C++ naming convention.
    Vector2 position_;
    float speed_ = 220.0f;   // World units (pixels) per second
    float radius_ = 16.0f;

    float maxHealth_ = 100.0f;
    float health_ = maxHealth_;

    // After getting hit, the player can't be hit again for a short time ("i-frames").
    // Without this, touching 10 enemies would drain all your health in a few frames.
    float invulnerableTimer_ = 0.0f;

    int level_ = 1;
    int experience_ = 0;              // XP collected toward the next level
    int experienceToNextLevel_ = 5;
    float pickupRadius_ = 90.0f;      // Gems inside this distance get pulled toward the player
};
