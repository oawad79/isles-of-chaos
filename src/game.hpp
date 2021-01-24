#ifndef SKYVAULT_GAME_H_
#define SKYVAULT_GAME_H_

#include <vector>
#include <entt.hpp>

#include "utils.hpp"
#include "sprite.hpp"
#include "scene.hpp"
#include "level.hpp"
#include "consts.hpp"

enum class AppState {
    Running,
    Paused,
    Stopped
};

struct Game {
    AppState state{AppState::Running};
    entt::registry reg;
    SpriteRenderer spriteRenderer;
    uptr<Level> level;

    const Color startBackgroundClearColor{0,0,0,0};
    Color backgroundClearColor{0, 0, 0, 0};

    bool physicsPaused{false};

    std::vector<SceneLayer*> scenes;

    Camera2D mainCamera = {0};
    RenderTexture2D mainCanvas;
};

void LoadGame(uptr<Game>& game);
void PushScene(uptr<Game>& game, SceneLayer* scene);
void PopScene(uptr<Game>& game);
void GotoScene(uptr<Game>& game, SceneLayer* scene);

void UpdateGame(uptr<Game>& game);
void RenderGame(const uptr<Game>& game);

Vector2 MouseCanvasPosition(const uptr<Game>& game);

#endif // SKYVAULT_GAME_H_
