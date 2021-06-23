#ifndef SKYVAULT_ACTOR_HPP
#define SKYVAULT_ACTOR_HPP

#include <entt.hpp>

#include "enttypes.hpp"
#include "physics_type.hpp"
#include "body.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "spawners.hpp"
#include "item.hpp"
#include "particles.hpp"

enum class ActorType {
    NPC,
    KIWI_BIRD,
    BUG,

    ENEMY_START,

    ZAMBIE,
    DREAD_SHARK,
    GHOST,
    SHROOMBA,

    ENEMY_END,
};

enum class ActorState {
    IDLE,
    FIND_NEW_TARGET,
    WONDER,
    WAITING,
    FALLING,
    TRACKING,
    JUMPING,
    JUMP,
    ATTACKING,
    DEAD,
};

enum class AiType {
  NONE,
};

struct Actor {
    ActorType type{ActorType::NPC};
    ActorName actorName{ActorName::Anon};

    AiType ai{AiType::NONE};
    ActorState state{ActorState::IDLE};

    Enemy enemyStats{};

    Vector2 target[8];
    float timer[8] = {0.0f};
};

bool IsEnemy(Actor actor);
void UpdateActor(entt::registry& reg);

#endif // SKYVAULT_ACTOR_HPP
