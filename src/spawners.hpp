#ifndef SKYVAULT_SPAWNERS_HPP
#define SKYVAULT_SPAWNERS_HPP

#include <map>
#include <functional>

#include <raylib.h>
#include <entt.hpp>

#include "game.hpp"
#include "utils.hpp"
#include "enttypes.hpp"

#include "player.hpp"
#include "body.hpp"
#include "physics.hpp"
#include "interaction.hpp"

using Spawner = std::function<
    entt::entity(
        const uptr<Game>& game,
        const Vector2 position
    )
>;

#define GENERATE_SPAWNER(T) { #T, Spawn##T },

entt::entity SpawnTest(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnPlayer(const uptr<Game>& game, const Vector2 position);

static std::map<const std::string, Spawner> SpawnerMap = {
  X_ENT_TYPES(GENERATE_SPAWNER)
};

entt::entity Spawn(
    const std::string& which,
    const uptr<Game>& game,
    const Vector2 position);

#undef GENERATE_SPAWNER

#endif // SKYVAULT_SPAWNERS_HPP
