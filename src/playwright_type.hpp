#ifndef ISLESOFCHAOS_PLAYWRITE_TYPE_HPP
#define ISLESOFCHAOS_PLAYWRITE_TYPE_HPP

#include <optional>
#include <set>
#include <entt.hpp>

#include "enttypes.hpp"

struct Action {
    std::set<ActorName> actors;
    std::function<bool(entt::registry&, const entt::entity&)> tick;
};

struct Script {
    std::vector<Action> actions;
    std::function<bool(entt::registry&, const entt::entity&)> onInit;
    int actionNumber{0};
};

struct Play {
    Script script;
};

struct Stage {
    std::optional<Play> currentPlay;
};


#endif//ISLESOFCHAOS_PLAYWRITE_TYPE_HPP