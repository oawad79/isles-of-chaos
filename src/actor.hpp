#ifndef SKYVAULT_ACTOR_HPP
#define SKYVAULT_ACTOR_HPP

#include <entt.hpp>

#include "enttypes.hpp"
#include "physics_type.hpp"
#include "body.hpp"
#include "player.hpp"
#include "item.hpp"

enum class ActorType {
    NPC,
    ENEMY_START,

    ZAMBIE,

    ENEMY_END,
};

enum class ActorState {
    IDLE,
    WONDER,
    WAITING,
    TRACKING,
    JUMPING,
    ATTACKING,
};

enum class AiType {
  NONE,
};

struct Actor {
    ActorType type{ActorType::NPC};
    AiType ai{AiType::NONE};
    ActorState state{ActorState::IDLE};
    Vector2 target[8];
    float timer[8];
};

void UpdateActor(entt::registry& reg);

#endif // SKYVAULT_ACTOR_HPP
