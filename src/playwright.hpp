#ifndef ISLESOFCHAOS_PLAYWRIGHT_HPP
#define ISLESOFCHAOS_PLAYWRIGHT_HPP

#include <optional>
#include <set>
#include <entt.hpp>

#include "actor.hpp"
#include "enttypes.hpp"

struct Action {
    std::set<ActorName> actors;
    std::function<bool(entt::registry&, const entt::entity&)> tick;
};

struct Script {
    std::vector<Action> actions;
    int actionNumber{0};
};

struct Play {
    Script script;
};

struct Stage {
    std::optional<Play> currentPlay;
};

Play MakeDemoPlay();

void DoScreenPlay(Stage& stage, Play& play);

void UpdatePlaywright(Stage& stage, entt::registry& reg);
void DrawPlaywright(Stage& stage);

#endif //ISLESOFCHAOS_PLAYWRIGHT_HPP
