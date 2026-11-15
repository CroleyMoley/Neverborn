#pragma once

#include <raylib.h>

// A basic enemy that walks straight toward a target (the player).
class Enemy
{
public:
    Enemy(Vector2 position, float speed, float radius, float health);

    void update(float dt, Vector2 target);
    void draw() const;

    // Moves the enemy by an offset. Used to push overlapping enemies apart and for knockback.
    void push(Vector2 offset);

    void takeDamage(float amount);

    Vector2 position() const { return position_; }
    float radius() const { return radius_; }
    bool isDead() const { return health_ <= 0.0f; }

private:
    Vector2 position_;
    float speed_;
    float radius_;
    float health_;
    float hitFlashTimer_ = 0.0f;  // Briefly draw the enemy white after it gets hit
};
