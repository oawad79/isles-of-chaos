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
#include "timed.hpp"
#include "character.hpp"
#include "actor.hpp"
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

void Update(uptr<Game>& game) {
    UpdateGame(game);
    UpdateGui(game);

    if (game->state == AppState::Running) {
        UpdateSprites(game->reg);
        UpdatePlayer(game, game->reg);
        UpdatePhysics(game, game->reg);
        UpdateTimed(game->reg);
        UpdateCharacter(game->reg);
        UpdateActor(game->reg);
        UpdateWater(game->reg);
        UpdateInteraction(game, game->reg);
    }
}

void Render(const uptr<Game>& game) {
    RenderGame(game);
    const auto* tilemap = GetTilemap(game->level);
    if (tilemap != nullptr)
        DrawTilemapToTarget(tilemap, game->mainCamera, game->spriteRenderer);
    BeginTextureMode(game->mainCanvas);
        if (tilemap != nullptr) game->backgroundClearColor = tilemap->backgroundColor;
        ClearBackground(game->backgroundClearColor);

        if (tilemap && tilemap->backgroundColor.a == 0)
            DrawTextureEx(
                Assets::I()->textures[TEX_BG],
                {0,0},0.0f,6.0f,
                WHITE);

        if (tilemap != nullptr) DrawTilemap(tilemap);
        BeginMode2D(game->mainCamera);
            DrawSprites(game->spriteRenderer, game->reg);
            DrawWater(game->reg);
            DrawInteraction(game, game->reg);
            if (IsKeyDown(KEY_TAB))
                DrawDebugPhysicsInfo(game, game->reg);
        EndMode2D(); 
    EndTextureMode();

    BeginTextureMode(game->guiCanvas); 
        ClearBackground({0, 0, 0, 0});
        RenderGui(game); 
    EndTextureMode();
}

int main(const int argc, const char *argv[]) {
    std::cout << "Hello Isles of Chaos!" << std::endl;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(1280, 720, "DevWindow");
    SetTargetFPS(60);
    //HideCursor();

//    SetExitKey(0);

    auto game = std::make_unique<Game>();

    LoadGame(game);
    LoadAllAssets();

    GotoScene(game, new GameScene(game->reg));

    while (!WindowShouldClose() && game->state != AppState::Stopped) {
        const auto screenWidth = GetScreenWidth();
        const auto screenHeight = GetScreenHeight();

        Update(game);
        Render(game);

        BeginDrawing();
            ClearBackground(BLACK);
            // DrawTexture(game->mainCanvas.texture, 0, 0, WHITE);
            const float aspect = (float)CANVAS_HEIGHT / (float)CANVAS_WIDTH;
            const float width = GetScreenWidth();
            const float height = width * aspect;

            const float x = screenWidth / 2.0f - width / 2.0f;
            const float y = screenHeight / 2.0f - height / 2.0f;

            {
              const auto tex = game->mainCanvas.texture; 
              DrawTexturePro(
                  tex,
                  {0,0,(float)tex.width, -(float)tex.height},
                  {x,y,width,height},
                  Vector2Zero(),
                  0.0f,
                  WHITE);
            }

            {
              const auto tex = game->guiCanvas.texture; 
              const float aspect = (float)GUI_CANVAS_WIDTH / (float)GUI_CANVAS_HEIGHT;
              DrawTexturePro(
                  tex,
                  {0,0,(float)tex.width, -(float)tex.height},
                  {0,0,(GetScreenHeight()*aspect),(float)GetScreenHeight()},
                  Vector2Zero(),
                  0.0f,
                  WHITE);
            }

            DrawFPS(0, 0);
        EndDrawing();
    }

    return 0;
}
