#include "Enemy.h"

#include <raymath.h>

namespace
{
constexpr Color kEnemyColor{200, 60, 80, 255};
constexpr Color kEnemyOutline{90, 20, 35, 255};
}  // namespace

Enemy::Enemy(Vector2 position, float speed, float radius)
    : position_(position)
    , speed_(speed)
    , radius_(radius)
{
}

void Enemy::update(float dt, Vector2 target)
{
    // Direction = (target - position), scaled to length 1.
    const Vector2 direction = Vector2Normalize(Vector2Subtract(target, position_));
    position_ = Vector2Add(position_, Vector2Scale(direction, speed_ * dt));
}

void Enemy::push(Vector2 offset)
{
    position_ = Vector2Add(position_, offset);
}

void Enemy::draw() const
{
    DrawCircleV(position_, radius_, kEnemyColor);
    DrawCircleLines(static_cast<int>(position_.x), static_cast<int>(position_.y), radius_, kEnemyOutline);
}
