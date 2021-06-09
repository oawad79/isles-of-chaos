#include "playwright.hpp"

Play MakeDemoPlay(entt::registry& reg) {
}

void UpdatePlaywright(Stage& stage) {
    if (!stage.currentPlay.has_value()) return;

    auto& play = stage.currentPlay.value();
}

void DrawPlaywright(Stage& stage) {

}
