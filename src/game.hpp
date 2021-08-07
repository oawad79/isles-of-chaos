#ifndef SKYVAULT_GAME_H_
#define SKYVAULT_GAME_H_

#include <vector>
#include <optional>
#include <filesystem>
#include <fstream>

#include <entt.hpp>

#include "tinyxml2.hpp"
#include "gif.hpp"
#include "utils.hpp"
#include "sprite.hpp"
#include "gui_state.hpp"
#include "scene.hpp"
#include "dialog.hpp"
#include "level.hpp"
#include "consts.hpp"

#include "playwright_type.hpp"

constexpr float GIF_WIDTH { CANVAS_WIDTH * 2.0f };
constexpr float GIF_HEIGHT { CANVAS_HEIGHT * 2.0f };

enum class AppState {
    Running,
    Paused,
    PauseMenu,
    InDialog,
    InCutscene,
    Stopped
};

struct Game {
    // TODO(Dustin): We need to make game states a stack, because nested menus will
    // want to know how to jump back, right now we can only go back 1 step, eg. pause, to unpause
    AppState state{AppState::Running};
    AppState lastState{};

    entt::registry reg;
    SpriteRenderer spriteRenderer;
    uptr<Level> level;

    const Color startBackgroundClearColor{0,0,0,0};
    Color backgroundClearColor{0, 0, 0, 0};

    bool physicsPaused{false};

    std::vector<SceneLayer*> scenes;

    float timeInGame{60.0f};

    Camera2D mainCamera = {0};
    RenderTexture2D mainCanvas;
    RenderTexture2D guiCanvas;
    GuiState guiState;

    Vector2 respawnLocation;

    Stage stage;
    std::vector<std::vector<uint8_t>> totalFrames;
    float frameTimer = 0.0f;

    float shade {0.0f};

    bool recordingGif{false};
    GifWriter gifWriter;

    int shaderTimeLoc = -1;

    std::optional<DialogTree> dialogTree{std::nullopt};
};

void LoadGame(uptr<Game>& game);
void PushScene(const uptr<Game>& game, SceneLayer* scene);
void PopScene(const uptr<Game>& game);
void GotoScene(const uptr<Game>& game, SceneLayer* scene);

bool SaveGameState(const uptr<Game>& game, const std::string& name);
bool LoadGameState(const uptr<Game>& game, const std::string& name);

void UpdateGame(uptr<Game>& game);
void RenderGame(const uptr<Game>& game);

void DoDialog(const uptr<Game>& game, const DialogTree& tree);

Vector2 MouseCanvasPosition(const uptr<Game>& game);

#endif // SKYVAULT_GAME_H_
