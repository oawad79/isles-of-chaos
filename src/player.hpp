#ifndef SKYVAULT_PLAYER_HPP
#define SKYVAULT_PLAYER_HPP

#include <entt.hpp>

#include "sprite.hpp"
#include "body.hpp"
#include "physics.hpp"
#include "input.hpp"

struct Player {
    int health{3};
};

entt::entity SpawnPlayer(entt::registry& reg, float x, float y);

void UpdatePlayer(entt::registry& reg);

#endif // SKYVAULT_PLAYER_HPP
