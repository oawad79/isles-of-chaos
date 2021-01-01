#ifndef SKYVAULT_GAME_H_
#define SKYVAULT_GAME_H_

#include <entt.hpp>

#include "utils.hpp"
#include "sprite.hpp"
#include "tilemap.hpp"

constexpr auto CANVAS_WIDTH { 256 };
constexpr auto CANVAS_HEIGHT { 144 };

enum class AppState {
    Running,
    Paused,
    Stopped
};

struct Game {
    AppState state{AppState::Running};
    entt::registry reg;
    SpriteRenderer spriteRenderer;
    uptr<Tilemap> tilemap;

    Camera2D mainCamera = {0};
    RenderTexture2D mainCanvas;
};

void LoadGame(uptr<Game>& game);

#endif // SKYVAULT_GAME_H_
