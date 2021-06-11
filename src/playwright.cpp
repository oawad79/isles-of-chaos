#include "playwright.hpp"

Play MakeDemoPlay(entt::registry& reg) {
    return {};
}

void DoScreenPlay(Stage& stage, Play& play) {
    stage.currentPlay = play;
    stage.currentPlay->script.actionNumber = 0;
}

void UpdatePlaywright(Stage& stage, entt::registry& reg) {
    if (!stage.currentPlay.has_value()) return;
    auto& play = stage.currentPlay.value();
    auto& action = play.script.actions[play.script.actionNumber];

    auto view = reg.view<Actor>();
    for (const entt::entity& e : view) {
        auto& name = reg.get<Actor>(e).actorName;

        if (action.actors.find(name) != action.actors.end()) {
            if (play.script.actions[play.script.actionNumber].tick(reg, e)) {
                play.script.actionNumber += 1;
                if (play.script.actionNumber >= play.script.actions.size())
                    stage.currentPlay = std::nullopt;
            }
        }
    }
}

void DrawPlaywright(Stage& stage) {
    if (!stage.currentPlay.has_value()) return;

}
