#include "Enemy.h"

#include <algorithm>

#include <raymath.h>

namespace
{
    constexpr Color kEnemyColor{ 200, 60, 80, 255 };
    constexpr Color kEnemyHitColor{ 255, 255, 255, 255 };
    constexpr Color kEnemyOutline{ 90, 20, 35, 255 };
    constexpr float kHitFlashDuration = 0.1f;
}  // namespace

Enemy::Enemy(Vector2 position, float speed, float radius, float health)
    : position_(position)
    , speed_(speed)
    , radius_(radius)
    , health_(health)
{
}

void Enemy::update(float dt, Vector2 target)
{
    hitFlashTimer_ = std::max(0.0f, hitFlashTimer_ - dt);

    // Direction = (target - position), scaled to length 1.
    const Vector2 direction = Vector2Normalize(Vector2Subtract(target, position_));
    position_ = Vector2Add(position_, Vector2Scale(direction, speed_ * dt));
}

void Enemy::push(Vector2 offset)
{
    position_ = Vector2Add(position_, offset);
}

void Enemy::takeDamage(float amount)
{
    health_ -= amount;
    hitFlashTimer_ = kHitFlashDuration;
}

void Enemy::draw() const
{
    const Color color = hitFlashTimer_ > 0.0f ? kEnemyHitColor : kEnemyColor;
    DrawCircleV(position_, radius_, color);
    DrawCircleLines(static_cast<int>(position_.x), static_cast<int>(position_.y), radius_, kEnemyOutline);
}
