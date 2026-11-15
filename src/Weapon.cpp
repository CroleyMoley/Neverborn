#include "Weapon.h"

#include <raymath.h>

Weapon::Weapon(WeaponStats stats)
    : stats_(stats)
{
}

void Weapon::update(float dt, Vector2 origin, const std::vector<Enemy>& enemies,
                    std::vector<Projectile>& projectiles)
{
    cooldownTimer_ -= dt;
    if (cooldownTimer_ > 0.0f)
    {
        return;
    }

    const Enemy* target = findNearestEnemy(origin, enemies);
    if (target == nullptr)
    {
        // Nothing in range. Stay ready so we fire the instant an enemy gets close.
        cooldownTimer_ = 0.0f;
        return;
    }

    fire(origin, target->position(), projectiles);  // `->` accesses a member through a pointer
    cooldownTimer_ = stats_.cooldown;
}

const Enemy* Weapon::findNearestEnemy(Vector2 origin, const std::vector<Enemy>& enemies) const
{
    const Enemy* nearest = nullptr;
    float nearestDistanceSq = stats_.range * stats_.range;

    for (const Enemy& enemy : enemies)
    {
        const float distanceSq = Vector2DistanceSqr(origin, enemy.position());
        if (distanceSq < nearestDistanceSq)
        {
            nearestDistanceSq = distanceSq;
            nearest = &enemy;  // `&enemy` = the address of this enemy (a pointer to it)
        }
    }
    return nearest;
}

void Weapon::fire(Vector2 origin, Vector2 target, std::vector<Projectile>& projectiles) const
{
    const Vector2 aim = Vector2Normalize(Vector2Subtract(target, origin));

    // Fan the bolts out evenly around the aim direction.
    // e.g. 3 bolts with a 12 degree spread -> -12, 0, +12 degrees.
    const float spreadRadians = stats_.spreadDegrees * DEG2RAD;
    const float firstOffset = -0.5f * static_cast<float>(stats_.projectileCount - 1) * spreadRadians;

    for (int i = 0; i < stats_.projectileCount; ++i)
    {
        const float angle = firstOffset + static_cast<float>(i) * spreadRadians;
        const Vector2 direction = Vector2Rotate(aim, angle);

        projectiles.emplace_back(origin,
                                 Vector2Scale(direction, stats_.projectileSpeed),
                                 stats_.projectileRadius,
                                 stats_.damage,
                                 stats_.projectileLifetime);
    }
}
