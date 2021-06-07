#ifndef SKYVAULT_GAME_H_
#define SKYVAULT_GAME_H_

#include <vector>
#include <entt.hpp>
#include <optional>

#include "utils.hpp"
#include "sprite.hpp"
#include "gui_state.hpp"
#include "scene.hpp"
#include "dialog.hpp"
#include "level.hpp"
#include "consts.hpp"

enum class AppState {
    Running,
    Paused,
    InDialog,
    InCutscene,
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
    RenderTexture2D guiCanvas;
    GuiState guiState;

    float shade {0.0f};

    std::optional<DialogTree> dialogTree{std::nullopt};
};

void LoadGame(uptr<Game>& game);
void PushScene(uptr<Game>& game, SceneLayer* scene);
void PopScene(uptr<Game>& game);
void GotoScene(uptr<Game>& game, SceneLayer* scene);

void UpdateGame(uptr<Game>& game);
void RenderGame(const uptr<Game>& game);

void DoDialog(const uptr<Game>& game, const DialogTree& tree);

Vector2 MouseCanvasPosition(const uptr<Game>& game);
Vector2 MouseGuiCanvasPosition(const uptr<Game>& game);

#endif // SKYVAULT_GAME_H_
