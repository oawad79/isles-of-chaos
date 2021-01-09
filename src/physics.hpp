#pragma once

#include <raylib.h>
#include <entt.hpp>
#include <cmath>
#include <vector>

#include "body.hpp"
#include "level.hpp"
#include "utils.hpp"
#include "game.hpp"

enum Facing {
    LEFT = -1,
    RIGHT = 1
};

struct Physics {
    Vector2 velocity{Vector2{0.0f, 0.0f}};
    float friction{0.01f};

    bool on_ground{false};
    bool on_ladder{false};

    bool colliding_with_solid{false};
    Vector2 solid_collision_point {Vector2{0, 0}};
    Facing facingX{RIGHT}; // Default facing right
};

void UpdatePhysics(uptr<Game>& game, entt::registry& reg);
void DrawDebugPhysicsInfo(const uptr<Game>& game, entt::registry& reg);
