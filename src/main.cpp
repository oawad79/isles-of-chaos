#include <iostream>
#include <mutex>
#include <memory>
#include <raylib.h>

#include "utils.hpp"
#include "level.hpp"
#include "game.hpp"
#include "tilemap.hpp"
#include "player.hpp"
#include "playwright.hpp"
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
#include "menu_scene.hpp"

uptr<Assets> Assets::it;
std::once_flag Assets::once;

uptr<Input> Input::it;
std::once_flag Input::once;

void Update(uptr<Game>& game) {
    UpdateGame(game);
    UpdateGui(game);

    if (game->state == AppState::Running) {
        UpdatePlaywright(game->stage);

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
    const auto* tilemap = GetTilemap(game->level);
    if (tilemap != nullptr)
        DrawTilemapToTarget(tilemap, game->mainCamera, game->spriteRenderer);
    BeginTextureMode(game->mainCanvas);
        if (tilemap != nullptr) game->backgroundClearColor = tilemap->backgroundColor;
        ClearBackground(game->backgroundClearColor);

        if (tilemap && tilemap->backgroundColor.a == 0) {
            DrawTextureEx(
                    Assets::I()->textures[TEX_BG],
                    {-256*4, -256*2},
                    0.0f,
                    12.0f,
                    WHITE);
        }

        if (tilemap != nullptr) DrawTilemap(tilemap);
        BeginMode2D(game->mainCamera);

//            BeginShaderMode(Assets::I()->shaders[SPRITE_SHADER]);
                DrawSprites(game->spriteRenderer, game->reg);
//            EndShaderMode();

            DrawWater(game->reg);
            DrawInteraction(game, game->reg);
            if (IsKeyDown(KEY_TAB))
                DrawDebugPhysicsInfo(game, game->reg);

            DrawPlaywright(game->stage);
        EndMode2D();
    EndTextureMode();

    BeginTextureMode(game->guiCanvas); 
        ClearBackground({0, 0, 0, 0});
        RenderGui(game);
        RenderGame(game);
    EndTextureMode();
}

int main(const int argc, const char *argv[]) {
    std::cout << "Hello Isles of Chaos!" << std::endl;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(1280, 720, "DevWindow");
    SetWindowState(FLAG_WINDOW_ALWAYS_RUN);
    SetTargetFPS(85);
    //HideCursor();

//    SetExitKey(0);

    auto game = std::make_unique<Game>();

    LoadGame(game);
    LoadAllAssets();

    LoadSpriteRenderer(game->spriteRenderer);

//    GotoScene(game, new MenuScene(game->reg));
    GotoScene(game, new GameScene(game->reg));

    while (!WindowShouldClose() && game->state != AppState::Stopped) {
        const auto screenWidth = GetScreenWidth();
        const auto screenHeight = GetScreenHeight();

        if (IsWindowFocused() || true) {
            Update(game);
            Render(game);

            BeginDrawing();
            ClearBackground(BLACK);
            // DrawTexture(game->mainCanvas.texture, 0, 0, WHITE);
            const float aspect = (float) CANVAS_HEIGHT / (float) CANVAS_WIDTH;
            const float width = GetScreenWidth();
            const float height = width * aspect;

            const float x = screenWidth / 2.0f - width / 2.0f;
            const float y = screenHeight / 2.0f - height / 2.0f;

            {
                const auto tex = game->mainCanvas.texture;
                DrawTexturePro(
                        tex,
                        {0, 0, (float) tex.width, -(float) tex.height},
                        {x, y, width, height},
                        Vector2Zero(),
                        0.0f,
                        WHITE);
            }

            DrawRectangleRec({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                             {0, 0, 0, (unsigned char)(255.0f * game->shade)});

            {
                const auto tex = game->guiCanvas.texture;
                const float aspect = (float) GUI_CANVAS_WIDTH / (float) GUI_CANVAS_HEIGHT;
                DrawTexturePro(
                        tex,
                        {0, 0, (float) tex.width, -(float) tex.height},
                        {0, 0, (GetScreenHeight() * aspect), (float) GetScreenHeight()},
                        Vector2Zero(),
                        0.0f,
                        WHITE);
            }

            DrawFPS(GetScreenWidth() - 100, 0);
            EndDrawing();
        } else {
            BeginDrawing();
            ClearBackground(BLACK);
                DrawText("Click on window!", 0, 0, 120, WHITE);
            EndDrawing();
        }
    }

    return 0;
}
