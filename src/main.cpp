#include <iostream>
#include <mutex>
#include <memory>
#include <raylib.h>

#include "utils.hpp"
#include "level.hpp"
#include "game.hpp"
#include "tilemap.hpp"
#include "player.hpp"
#include "physics.hpp"
#include "spawners.hpp"
#include "interaction.hpp"
#include "assets.hpp"
#include "gui.hpp"
#include "input.hpp"
#include "enttypes.hpp"

#include "game_scene.hpp"

uptr<Assets> Assets::it;
std::once_flag Assets::once;

uptr<Input> Input::it;
std::once_flag Input::once;

GuiState guiState;

void Update(uptr<Game>& game) {
    if (game->state == AppState::Running) {
        UpdateSprites(game->reg);
        UpdatePlayer(game, game->reg);
        UpdatePhysics(game, game->reg);
        UpdateInteraction(game, game->reg);
    }

    UpdateGame(game);
    UpdateGui(game, guiState);
}

void RenderGui(const uptr<Game>& game) {
    DrawGui(game, guiState);
}

void Render(const uptr<Game>& game) {
    RenderGame(game);
    const auto* tilemap = GetTilemap(game->level);
    if (tilemap != nullptr)
        DrawTilemapToTarget(tilemap, game->mainCamera, game->spriteRenderer);
    BeginTextureMode(game->mainCanvas);
        ClearBackground({0, 0, 0, 0});
        DrawTexturePro(
            Assets::I()->textures[TEX_BG],
            {0,0,256, 144},
            {0,0,CANVAS_WIDTH,CANVAS_HEIGHT},
            Vector2Zero(),
            0.0f,
            WHITE);
        if (tilemap != nullptr) DrawTilemap(tilemap);
        BeginMode2D(game->mainCamera);
            DrawSprites(game->spriteRenderer, game->reg);
            DrawInteraction(game, game->reg);
            if (IsKeyDown(KEY_TAB))
                DrawDebugPhysicsInfo(game, game->reg);
        EndMode2D();
        RenderGui(game);
    EndTextureMode();
}

int main(const int argc, const char *argv[]) {
    std::cout << "Hello Isles of Chaos!" << std::endl;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    InitWindow(1280, 720, "Hello World");
    // SetTargetFPS(60);
    HideCursor();

    auto game = std::make_unique<Game>();

    LoadGame(game);
    LoadAllAssets();

    GotoScene(game, new GameScene());

    while (!WindowShouldClose() && game->state != AppState::Stopped) {
        const auto screenWidth = GetScreenWidth();
        const auto screenHeight = GetScreenHeight();

        Update(game);
        Render(game);

        BeginDrawing();
            ClearBackground(BLACK);
            // DrawTexture(game->mainCanvas.texture, 0, 0, WHITE);
            const auto tex = game->mainCanvas.texture;

            const float aspect = (float)CANVAS_HEIGHT / (float)CANVAS_WIDTH;
            const float width = GetScreenWidth();
            const float height = width * aspect;

            const float x = screenWidth / 2.0f - width / 2.0f;
            const float y = screenHeight / 2.0f - height / 2.0f;

            DrawTexturePro(
                tex,
                {0,0,(float)tex.width, -(float)tex.height},
                {x,y,width,height},
                Vector2Zero(),
                0.0f,
                WHITE);
            DrawFPS(0, 0);
        EndDrawing();
    }

    return 0;
}
