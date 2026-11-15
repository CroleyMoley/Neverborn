#pragma once

#include <raylib.h>

// A basic enemy that walks straight toward a target (the player).
class Enemy
{
public:
    Enemy(Vector2 position, float speed, float radius);

    void update(float dt, Vector2 target);
    void draw() const;

    // Moves the enemy by an offset. Used to push overlapping enemies apart.
    void push(Vector2 offset);

    Vector2 position() const { return position_; }
    float radius() const { return radius_; }

private:
    Vector2 position_;
    float speed_;
    float radius_;
};
