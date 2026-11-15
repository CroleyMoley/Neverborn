#pragma once

#include <vector>

#include <raylib.h>

#include "Enemy.h"
#include "Projectile.h"

// All the numbers that define a weapon, kept as plain data.
// Level-up upgrades will change these values, and later we'll load them from JSON files
// so new weapons can be added without changing code ("data-driven design").
struct WeaponStats
{
    float cooldown = 0.7f;             // Seconds between shots
    float damage = 10.0f;
    float projectileSpeed = 520.0f;
    float projectileRadius = 5.0f;
    float projectileLifetime = 1.2f;   // Seconds before a missed bolt disappears
    float range = 480.0f;              // Only fires at enemies closer than this
    int projectileCount = 1;           // Extra bolts fan out in a spread
    float spreadDegrees = 12.0f;       // Angle between bolts when projectileCount > 1
};

// Fires automatically at the nearest enemy whenever its cooldown is ready.
class Weapon
{
public:
    explicit Weapon(WeaponStats stats);

    // Parameter types matter here:
    //   const std::vector<Enemy>&  -> read-only access to the enemies, without copying the vector
    //   std::vector<Projectile>&   -> the weapon can ADD projectiles to the game's list
    void update(float dt, Vector2 origin, const std::vector<Enemy>& enemies,
                std::vector<Projectile>& projectiles);

    const WeaponStats& stats() const { return stats_; }
    WeaponStats& stats() { return stats_; }  // Non-const version so upgrades can modify stats

private:
    // Returns a pointer to the closest enemy, or nullptr if none is in range.
    // A pointer is used because "no enemy found" is a valid answer (like returning null in Java).
    const Enemy* findNearestEnemy(Vector2 origin, const std::vector<Enemy>& enemies) const;

    void fire(Vector2 origin, Vector2 target, std::vector<Projectile>& projectiles) const;

    WeaponStats stats_;
    float cooldownTimer_ = 0.0f;
};
