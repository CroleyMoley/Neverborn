#pragma once

#include <raylib.h>

// A bullet/bolt that flies in a straight line until it hits something or its lifetime runs out.
class Projectile
{
public:
    Projectile(Vector2 position, Vector2 velocity, float radius, float damage, float lifetime);

    void update(float dt);
    void draw() const;

    // Called when the projectile hits an enemy, so it gets removed.
    void markHit() { hasHit_ = true; }

    Vector2 position() const { return position_; }
    Vector2 direction() const;  // Unit vector the projectile is travelling in
    float radius() const { return radius_; }
    float damage() const { return damage_; }
    bool isExpired() const { return hasHit_ || lifetime_ <= 0.0f; }

private:
    Vector2 position_;
    Vector2 velocity_;   // Pixels per second, in x and y
    float radius_;
    float damage_;
    float lifetime_;     // Seconds left before it disappears
    bool hasHit_ = false;
};
