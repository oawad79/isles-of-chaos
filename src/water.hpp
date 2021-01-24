#ifndef SKYVAULT_WATER_HPP
#define SKYVAULT_WATER_HPP

#include "entt.hpp"
#include "particles.hpp"
#include "body.hpp"
#include "physics.hpp"
#include "assets.hpp"

struct Water {};

void UpdateWater(entt::registry& reg);
void DrawWater(entt::registry& reg);

#endif // SKYVAULT_WATER_HPP
