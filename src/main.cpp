#include <iostream>
#include <mutex>
#include <memory>
#include <raylib.h>

#include "utils.hpp"
#include "game.hpp"
#include "tilemap.hpp"
#include "player.hpp"
#include "physics.hpp"
#include "assets.hpp"
#include "input.hpp"

uptr<Assets> Assets::it;
std::once_flag Assets::once;

uptr<Input> Input::it;
std::once_flag Input::once;

int main(const int argc, const char *argv[]) {
    std::cout << "Hello Isles of Chaos!" << std::endl;

    InitWindow(1280, 720, "Hello World");

    auto game = std::make_unique<Game>();

    LoadGame(game);
    LoadAllAssets();

    game->tilemap = LoadTilemap(game->reg, "resources/maps/TestArea.tmx");

    SpawnPlayer(game->reg, 30, 20);

    while (!WindowShouldClose() && game->state != AppState::Stopped) {
        UpdateSprites(game->reg);
        UpdatePlayer(game->reg);
        UpdatePhysics(game, game->reg);

        BeginTextureMode(game->mainCanvas);
        ClearBackground(DARKBROWN);

        DrawText("Hello Isles of Chaos!", 100, 100, 20, WHITE);
        DrawSprites(game->spriteRenderer, game->reg);

        if (game->tilemap.get() != nullptr)
            DrawTilemap(game->tilemap, game->spriteRenderer);

        if (IsKeyDown(KEY_TAB)) {
            DrawDebugPhysicsInfo(game, game->reg);
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(game->mainCanvas.texture, 0, 0, WHITE);

        const auto tex = game->mainCanvas.texture;
        DrawTexturePro(
            tex,
            {0,0,(float)tex.width, -(float)tex.height},
            {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()},
            Vector2Zero(),
            0.0f,
            WHITE);

        EndDrawing();
    }

    return 0;
}
