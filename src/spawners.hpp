#ifndef SKYVAULT_SPAWNERS_HPP
#define SKYVAULT_SPAWNERS_HPP

#include <vector>
#include <functional>

#include <raylib.h>
#include <entt.hpp>

#include "game.hpp"
#include "utils.hpp"
#include "enttypes.hpp"
#include "tilemap.hpp"

#include "character.hpp"
#include "actor.hpp"
#include "player.hpp"
#include "body.hpp"
#include "physics_type.hpp"
#include "interaction.hpp"
#include "inventory.hpp"

using Spawner = std::function<
    entt::entity(
        const uptr<Game>& game,
        const Vector2 position
    )
>;

#define GENERATE_SPAWNER(T) { Spawn##T },

entt::entity SpawnNone(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnTest(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnGhost(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnPlayer(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnPot(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnItem(const uptr<Game>& game, const Vector2 position);

entt::entity SpawnItemWithId(
  const uptr<Game>& game,
  const Vector2 position,
  const std::string& id);

void SpawnEntitiesFromTileMap(const Tilemap* map, const uptr<Game>& game);

static std::vector<Spawner> SpawnerMap = {
  X_ENT_TYPES(GENERATE_SPAWNER)
};

entt::entity Spawn(
    const EntType which,
    const uptr<Game>& game,
    const Vector2 position);

#undef GENERATE_SPAWNER

#endif // SKYVAULT_SPAWNERS_HPP
