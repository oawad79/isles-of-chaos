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

#include "dialog.hpp"
#include "loot.hpp"
#include "character.hpp"
#include "actor.hpp"
#include "particles.hpp"
#include "player.hpp"
#include "body.hpp"
#include "water.hpp"
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
entt::entity SpawnZambie(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnShroomba(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnGhost(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnDreadShark(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnPlayer(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnPot(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnChest(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnWater(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnNpc(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnNpcWithId(const uptr<Game>& game, const Vector2 position, const std::string& id);

entt::entity SpawnItem(const uptr<Game>& game, const Vector2 position);
entt::entity SpawnItemWithId(
  entt::registry& reg,
  const Vector2 position,
  const std::string& id);

void SpawnLoot(entt::registry& reg, const Loot& loot, Vector2 where, Range1D xRange, Range2D velocityRange);

entt::entity SpawnParticle(const uptr<Game>& game, const Vector2 position);

void SpawnEntitiesFromTileMap(Tilemap* map, const uptr<Game>& game);

static std::vector<Spawner> SpawnerMap = {
  X_ENT_TYPES(GENERATE_SPAWNER)
};

entt::entity Spawn(
    const EntType which,
    const uptr<Game>& game,
    const Vector2 position);

#undef GENERATE_SPAWNER

#endif // SKYVAULT_SPAWNERS_HPP
