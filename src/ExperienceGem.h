#pragma once

#include <raylib.h>

// Dropped by enemies when they die. Walk near it and it flies to you.
//
// Java note: a `struct` is the same as a `class` in C++, except its members are public
// by default. By convention, structs are used for simple bundles of data with little behavior.
struct ExperienceGem
{
    Vector2 position;
    int value = 1;
    bool collected = false;
};
