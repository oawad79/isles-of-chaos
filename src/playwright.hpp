#ifndef ISLESOFCHAOS_PLAYWRIGHT_HPP
#define ISLESOFCHAOS_PLAYWRIGHT_HPP

#include <optional>
#include <set>
#include <entt.hpp>

#include "playwright_type.hpp"
#include "enttypes.hpp"
#include "actor.hpp"

Play MakeDemoPlay();

void DoScreenPlay(entt::registry& reg, Stage& stage, Play& play);

void UpdatePlaywright(Stage& stage, entt::registry& reg);
void DrawPlaywright(Stage& stage);

#endif //ISLESOFCHAOS_PLAYWRIGHT_HPP
