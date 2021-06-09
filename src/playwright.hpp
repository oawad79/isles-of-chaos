#ifndef ISLESOFCHAOS_PLAYWRIGHT_HPP
#define ISLESOFCHAOS_PLAYWRIGHT_HPP

#include <optional>
#include <vector>
#include <map>
#include <entt.hpp>

enum Actors {
    None, One, Two, Three, Four, Five, Six
};

struct Step {
    Actors who{None};
    bool async{false};
};

struct Script {
    std::vector<Step> steps;
};

struct Play {
    std::map<Actors, entt::entity> actors;
    Script script;
};

struct Stage {
    std::optional<Play> currentPlay;
};

Play MakeDemoPlay();

void UpdatePlaywright(Stage& stage);
void DrawPlaywright(Stage& stage);

#endif //ISLESOFCHAOS_PLAYWRIGHT_HPP
