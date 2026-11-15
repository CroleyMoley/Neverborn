#include "Projectile.h"

#include <raymath.h>

namespace
{
constexpr Color kProjectileColor{120, 200, 255, 255};
constexpr Color kProjectileGlow{120, 200, 255, 70};
}  // namespace

Projectile::Projectile(Vector2 position, Vector2 velocity, float radius, float damage, float lifetime)
    : position_(position)
    , velocity_(velocity)
    , radius_(radius)
    , damage_(damage)
    , lifetime_(lifetime)
{
}

void Projectile::update(float dt)
{
    position_ = Vector2Add(position_, Vector2Scale(velocity_, dt));
    lifetime_ -= dt;
}

Vector2 Projectile::direction() const
{
    return Vector2Normalize(velocity_);
}

void Projectile::draw() const
{
    DrawCircleV(position_, radius_ * 2.0f, kProjectileGlow);  // Soft glow behind the bolt
    DrawCircleV(position_, radius_, kProjectileColor);
}
