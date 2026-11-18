#pragma once

#include <functional>
#include <string>
#include <vector>

// Forward declarations: this header only mentions Player and Weapon by reference,
// so it doesn't need their full definitions. This keeps compile times down and avoids
// headers including each other in circles. Upgrade.cpp includes the real headers.
class Player;
class Weapon;

// One choice offered on level-up.
struct Upgrade
{
    std::string name;
    std::string description;
    int maxLevel;  // How many times this upgrade can be taken

    // std::function holds any callable, here a lambda that changes the player's or weapon's stats.
    // It's like a Java functional interface, e.g. BiConsumer<Player, Weapon>.
    std::function<void(Player&, Weapon&)> apply;
};

// Every upgrade in the game. Upgrades are referred to by their index in this list.
const std::vector<Upgrade>& allUpgrades();
