#ifndef SKYVAULT_ACTOR_HPP
#define SKYVAULT_ACTOR_HPP

#include <entt.hpp>
#include <cmath>

#include "enttypes.hpp"
#include "physics_type.hpp"
#include "body.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "spawners.hpp"
#include "item.hpp"
#include "particles.hpp"

constexpr auto BASE_STUN_TIME{0.25f};
 
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
    STUNNED,
    SITTING,
    LOOKING_TO_SIT,
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

    //float values[4] = {0.0f};
    float timer[4] = {0.0f};

    float stunTimer = 0.0f;
    ActorState last{ActorState::IDLE};
};

void Stun(Actor& actor);

bool IsEnemy(Actor actor);
void UpdateActor(entt::registry& reg);

#endif // SKYVAULT_ACTOR_HPP
