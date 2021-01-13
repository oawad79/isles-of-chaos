#pragma once

#include <vector>
#include <functional>
#include <entt.hpp>

#include "assets.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "player.hpp"

enum InteractionMode {
    CALL_WHEN_ENTERED,
    CALL_WHEN_EXITED,
    CALL_WHEN_INTERACTED,
};

struct Interaction {
    InteractionMode mode {CALL_WHEN_INTERACTED};
    bool last_intersection{false};
    std::function<void(entt::entity, entt::registry&)> action;
};

void UpdateInteraction(uptr<Game>& game, entt::registry& reg);
void DrawInteraction(const uptr<Game>& game, entt::registry& reg);
