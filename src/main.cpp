#include <iostream>
#include <mutex>
#include <memory>
#include <raylib.h>

#include "utils.hpp"
#include "game.hpp"
#include "tilemap.hpp"
#include "player.hpp"
#include "physics.hpp"
#include "spawners.hpp"
#include "interaction.hpp"
#include "assets.hpp"
#include "input.hpp"
#include "enttypes.hpp"

uptr<Assets> Assets::it;
std::once_flag Assets::once;

uptr<Input> Input::it;
std::once_flag Input::once;

void Update(uptr<Game>& game) {
    UpdateSprites(game->reg);
    UpdatePlayer(game, game->reg);
    UpdatePhysics(game, game->reg);
    UpdateInteraction(game, game->reg);
}

void Render(const uptr<Game>& game) {
    DrawText("Hello Isles of Chaos!", 10, 10, 8, WHITE);
    DrawSprites(game->spriteRenderer, game->reg);

    if (game->tilemap.get() != nullptr)
        DrawTilemap(game->tilemap, game->spriteRenderer);

    DrawInteraction(game, game->reg);

    if (IsKeyDown(KEY_TAB)) {
        DrawDebugPhysicsInfo(game, game->reg);
    }
}

void RenderGUI(const uptr<Game>& game) {
}

int main(const int argc, const char *argv[]) {
    std::cout << "Hello Isles of Chaos!" << std::endl;

    InitWindow(1280, 720, "Hello World");
    SetTargetFPS(60);

    auto game = std::make_unique<Game>();

    LoadGame(game);
    LoadAllAssets();

    game->tilemap = LoadTilemap(game->reg, "resources/maps/TestArea.tmx");

    SpawnPlayer(game, {30, 20});
    SpawnTest(game, {60, 30});

    while (!WindowShouldClose() && game->state != AppState::Stopped) {
        Update(game);

        BeginTextureMode(game->mainCanvas);
            BeginMode2D(game->mainCamera);
                ClearBackground(SKYBLUE);
                Render(game);
                RenderGUI(game);
            EndMode2D();
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
            DrawFPS(0, 0);
        EndDrawing();
    }

    return 0;
}
