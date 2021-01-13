#ifndef SKYVAULT_PHYSICS_TYPE_HPP
#define SKYVAULT_PHYSICS_TYPE_HPP

#include <raylib.h>

enum Facing {
    LEFT = -1,
    RIGHT = 1
};

struct Physics {
    Vector2 velocity{Vector2{0.0f, 0.0f}};
    float friction{0.01f};

    bool on_ground{false};
    bool on_ladder{false};

    float on_ground_timer{0.0f};

    bool colliding_with_solid{false};
    Vector2 solid_collision_point {Vector2{0, 0}};
    Facing facingX{RIGHT}; // Default facing right
};

#endif // SKYVAULT_PHYSICS_TYPE_PP
