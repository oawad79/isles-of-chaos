#ifndef SKYVAULT_PHYSICS_TYPE_HPP
#define SKYVAULT_PHYSICS_TYPE_HPP

#include <raylib.h>
#include <cmath>

enum class PhysicsType {
    DYNAMIC,
    KINEMATIC,
    STATIC,
};

enum Facing {
    LEFT = -1,
    RIGHT = 1,
    UP = 2,
};

struct Physics {
    PhysicsType type{PhysicsType::DYNAMIC};

    Vector2 velocity{0.0f, 0.0f};
    float friction{0.01f};

    Vector2 gravityScale{1.0f, 1.0f};

    bool on_ground{false};
    bool on_ladder{false};

    float on_ground_timer{0.0f};

    bool colliding_with_solid{false};

    Vector2 solid_collision_point {Vector2{0, 0}};
    Vector2 solid_collision_point_left {Vector2{0, 0}};
    Vector2 solid_collision_point_right {Vector2{0, 0}};

    Facing facingX{RIGHT}; // Default facing right

    inline Vector2 Speed() {
        return {
            std::abs(velocity.x),
            std::abs(velocity.y)
        };
    }
};

#endif // SKYVAULT_PHYSICS_TYPE_PP
