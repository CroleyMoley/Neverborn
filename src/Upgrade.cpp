#include "Upgrade.h"

#include "Player.h"
#include "Weapon.h"

const std::vector<Upgrade>& allUpgrades()
{
    // A `static` local variable is created once, the first time this function runs,
    // and then reused on every later call.
    //
    // Each lambda below starts with [] (captures nothing) and takes the player and weapon
    // by reference, so its changes affect the real objects. Unused parameters are left unnamed.
    static const std::vector<Upgrade> upgrades = {
        {"Sharpened Bolts", "+25% damage", 5,
         [](Player&, Weapon& weapon) { weapon.stats().damage *= 1.25f; }},

        {"Rapid Fire", "Fire 15% faster", 5,
         [](Player&, Weapon& weapon) { weapon.stats().cooldown *= 0.85f; }},

        {"Split Shot", "+1 bolt per shot", 4,
         [](Player&, Weapon& weapon) { weapon.stats().projectileCount += 1; }},

        {"Far Sight", "+20% bolt speed and range", 3,
         [](Player&, Weapon& weapon) {
             WeaponStats& stats = weapon.stats();
             stats.projectileSpeed *= 1.2f;
             stats.range *= 1.2f;
         }},

        {"Swift Feet", "+10% movement speed", 5,
         [](Player& player, Weapon&) { player.stats().speed *= 1.1f; }},

        {"Vitality", "+20 max health, heal 20", 5,
         [](Player& player, Weapon&) {
             player.stats().maxHealth += 20.0f;
             player.heal(20.0f);
         }},

        {"Magnetism", "+40% gem pickup range", 3,
         [](Player& player, Weapon&) { player.stats().pickupRadius *= 1.4f; }},
    };
    return upgrades;
}
