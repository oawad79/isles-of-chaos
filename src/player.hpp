#ifndef SKYVAULT_PLAYER_HPP
#define SKYVAULT_PLAYER_HPP

#include <entt.hpp>

#include "sprite.hpp"
#include "particles.hpp"
#include "body.hpp"
#include "character.hpp"
#include "timed.hpp"
#include "physics.hpp"
#include "input.hpp"

struct PlayerHit {};

struct Player {
    int health{3};
    std::optional<entt::entity> hit;
};

entt::entity SpawnPlayerHit(uptr<Game>& game, Item& item, float x, float y, Facing facing=RIGHT);

void UpdatePlayer(uptr<Game>& game, entt::registry& reg);

#endif // SKYVAULT_PLAYER_HPP
