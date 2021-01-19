#ifndef SKYVAULT_TIMED_HPP
#define SKYVAULT_TIMED_HPP

#include <entt.hpp>
#include <raylib.h>

#include "enttypes.hpp"

struct TimedDestroy {
    float lifeTime{1.0f};
};

void UpdateTimed(entt::registry& reg);

#endif // SKYVAULT_TIMED_HPP
