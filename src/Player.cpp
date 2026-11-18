#include "Player.h"

#include <algorithm>

#include <raymath.h>  // raylib's vector math helpers (Vector2Add, Vector2Normalize, ...)

namespace
{
    constexpr Color kPlayerColor{ 240, 200, 90, 255 };
    constexpr Color kPlayerHurtColor{ 255, 255, 255, 255 };
    constexpr Color kPlayerOutline{ 90, 60, 20, 255 };

    constexpr float kInvulnerableDuration = 0.6f;  // Seconds of i-frames after each hit
    constexpr float kFlashRate = 20.0f;            // Flashes per second while invulnerable

    constexpr float kHealthBarWidth = 40.0f;
    constexpr float kHealthBarHeight = 5.0f;
    constexpr float kHealthBarGap = 8.0f;          // Space between the player and the bar
    constexpr Color kHealthBarBack{ 60, 20, 30, 255 };
    constexpr Color kHealthBarFill{ 90, 220, 110, 255 };

    // Each level needs this much more XP than the previous one: 5, 10, 15, 20...
    constexpr int kExperienceIncreasePerLevel = 5;
}  // namespace

Player::Player(Vector2 startPosition)
    : position_(startPosition)
{
}

void Player::update(float dt)
{
    invulnerableTimer_ = std::max(0.0f, invulnerableTimer_ - dt);

    // Build a direction from the keys being held.
    Vector2 input{ 0.0f, 0.0f };
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

bool Player::takeDamage(float amount)
{
    if (invulnerableTimer_ > 0.0f || isDead())
    {
        return false;
    }

    health_ = std::max(0.0f, health_ - amount);
    invulnerableTimer_ = kInvulnerableDuration;
    return true;
}

int Player::addExperience(int amount)
{
    experience_ += amount;

    int levelsGained = 0;
    while (experience_ >= experienceToNextLevel_)
    {
        experience_ -= experienceToNextLevel_;  // Leftover XP carries over to the next level
        ++level_;
        experienceToNextLevel_ += kExperienceIncreasePerLevel;
        ++levelsGained;
    }
    return levelsGained;
}

void Player::draw() const
{
    // While invulnerable, alternate between normal and white to show the player was hit.
    const bool flashing = invulnerableTimer_ > 0.0f
        && static_cast<int>(invulnerableTimer_ * kFlashRate) % 2 == 0;

    // Placeholder art: a circle. We'll swap in a sprite later.
    DrawCircleV(position_, radius_, flashing ? kPlayerHurtColor : kPlayerColor);
    DrawCircleLines(static_cast<int>(position_.x), static_cast<int>(position_.y), radius_, kPlayerOutline);

    drawHealthBar();
}

// A small bar under the player, drawn in world space so it moves with them.
void Player::drawHealthBar() const
{
    const Vector2 barPosition{ position_.x - kHealthBarWidth / 2.0f,
                              position_.y + radius_ + kHealthBarGap };
    const float fraction = health_ / maxHealth_;

    DrawRectangleV(barPosition, Vector2{ kHealthBarWidth, kHealthBarHeight }, kHealthBarBack);
    DrawRectangleV(barPosition, Vector2{ kHealthBarWidth * fraction, kHealthBarHeight }, kHealthBarFill);
}
