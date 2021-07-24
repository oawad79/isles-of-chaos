#ifndef SKYVAULT_PLAYER_HPP
#define SKYVAULT_PLAYER_HPP

#include <entt.hpp>

#include "sprite.hpp"
#include "particles.hpp"
#include "body.hpp"
#include "character.hpp"
#include "actor.hpp"
#include "timed.hpp"
#include "physics.hpp"
#include "input.hpp"

constexpr auto DODGEROLL_COOLOFF { 0.65f };
constexpr auto DODGEROLL_X_SPEED { 7000.0f };
constexpr auto SWING_ANIM_SPEED{32.0f};

enum PlayerState {
  NORMAL,
  ROLLING,
};

struct PlayerHit {};

struct Player {
    int health{3};
    float facing = 1.0f;

    std::optional<entt::entity> hit;
    std::optional<Vector2> target;

    float dodgeRollCooloff { 0.0f };
    float dodgeRollVel { 0.0f };

    float swingAnimTimer{0.0f};
    float attackCooloff{0.0f};

    bool isJumping{false};

    PlayerState state { PlayerState::NORMAL };
};

entt::entity SpawnPlayerHit(uptr<Game>& game, Item& item, float x, float y, Facing facing=RIGHT);

void UpdatePlayer(uptr<Game>& game, entt::registry& reg);

#endif // SKYVAULT_PLAYER_HPP
