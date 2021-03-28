#pragma once

#include <raylib.h>
#include <entt.hpp>
#include <cmath>
#include <vector>

#include "body.hpp"
#include "level.hpp"
#include "utils.hpp"
#include "physics_type.hpp"
#include "game.hpp"

void UpdatePhysics(uptr<Game>& game, entt::registry& reg);
void DrawDebugPhysicsInfo(const uptr<Game>& game, entt::registry& reg);
