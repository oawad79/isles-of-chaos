#ifndef SKYVAULT_ACTOR_HPP
#define SKYVAULT_ACTOR_HPP

#include <entt.hpp>

#include "physics_type.hpp"
#include "body.hpp"
#include "player.hpp"
#include "item.hpp"

enum class ActorType {
    NPC,
    ENEMY
};

enum class AState {
    IDLE,
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
    AState state{AState::IDLE};
};

void UpdateActor(entt::registry& reg);

#endif // SKYVAULT_ACTOR_HPP
