#include "Player.h"

#include <raymath.h>  // raylib's vector math helpers (Vector2Add, Vector2Normalize, ...)

namespace
{
constexpr Color kPlayerColor{240, 200, 90, 255};
constexpr Color kPlayerOutline{90, 60, 20, 255};
}  // namespace

Player::Player(Vector2 startPosition)
    : position_(startPosition)
{
}

void Player::update(float dt)
{
    // Build a direction from the keys being held.
    Vector2 input{0.0f, 0.0f};
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    input.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  input.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  input.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) input.x += 1.0f;

    // Without normalizing, moving diagonally would be ~41% faster (length of (1,1) is 1.414).
    // Vector2Normalize returns (0,0) safely when no key is pressed.
    const Vector2 direction = Vector2Normalize(input);

    // position += direction * speed * dt
    position_ = Vector2Add(position_, Vector2Scale(direction, speed_ * dt));
}

void Player::draw() const
{
    // Placeholder art: a circle. We'll swap in a sprite later.
    DrawCircleV(position_, radius_, kPlayerColor);
    DrawCircleLines(static_cast<int>(position_.x), static_cast<int>(position_.y), radius_, kPlayerOutline);
}
