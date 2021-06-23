#ifndef SKYVAULT_GAME_H_
#define SKYVAULT_GAME_H_

#include <vector>
#include <optional>
#include <filesystem>

#include <entt.hpp>

#include "gif.hpp"
#include "utils.hpp"
#include "sprite.hpp"
#include "gui_state.hpp"
#include "scene.hpp"
#include "dialog.hpp"
#include "level.hpp"
#include "consts.hpp"

#include "playwright_type.hpp"

constexpr int GIF_WIDTH { CANVAS_WIDTH * 2 };
constexpr int GIF_HEIGHT { CANVAS_HEIGHT * 2 };

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

    Stage stage;
    std::vector<std::vector<uint8_t>> totalFrames;
    float frameTimer = 0.0f;

    float shade {0.0f};

    bool recordingGif{false};
    GifWriter gifWriter;

    std::optional<DialogTree> dialogTree{std::nullopt};
};

void LoadGame(uptr<Game>& game);
void PushScene(uptr<Game>& game, SceneLayer* scene);
void PopScene(uptr<Game>& game);
void GotoScene(uptr<Game>& game, SceneLayer* scene);

bool SaveGameState(uptr<Game>& game, const std::string& name);
bool LoadGameState(uptr<Game>& game, const std::string& name);

void UpdateGame(uptr<Game>& game);
void RenderGame(const uptr<Game>& game);

void DoDialog(const uptr<Game>& game, const DialogTree& tree);

Vector2 MouseCanvasPosition(const uptr<Game>& game);
Vector2 MouseGuiCanvasPosition(const uptr<Game>& game);

#endif // SKYVAULT_GAME_H_
