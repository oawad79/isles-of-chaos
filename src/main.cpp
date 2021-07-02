#include <iostream>
#include <mutex>
#include <memory>
#include <set>
#include <raylib.h>

#include "window_sizing.hpp"
#include "utils.hpp"
#include "level.hpp"
#include "game.hpp"
#include "player.hpp"
#include "playwright.hpp"
#include "gui.hpp"

#include "game_scene.hpp"
#include "menu_scene.hpp"

uptr<Assets> Assets::it;
std::once_flag Assets::once;

uptr<Input> Input::it;
std::once_flag Input::once;

void Update(uptr<Game>& game) {
    UpdateGame(game);
    UpdateGui(game);
}

void Render(const uptr<Game>& game) {
    const auto* tilemap = GetTilemap(game->level);

    GetWindowSize(true);

    Camera2D cameraCopy = game->mainCamera;
    cameraCopy.target.x = floor(cameraCopy.target.x);
    cameraCopy.target.y = floor(cameraCopy.target.y);

  if (tilemap != nullptr)
        DrawTilemapToTarget(tilemap, cameraCopy, game->spriteRenderer);

    BeginTextureMode(game->mainCanvas);
        if (tilemap != nullptr) game->backgroundClearColor = tilemap->backgroundColor;
        ClearBackground(game->backgroundClearColor);

        if (tilemap && tilemap->backgroundColor.a == 0) {
            DrawTextureEx(
                Assets::I()->textures[TEX_BG],
                {0, -25},
                0.0f,
                1.0f,
                WHITE);
        }

        if (tilemap != nullptr) DrawTilemap(tilemap);
        BeginMode2D(game->mainCamera);
            DrawSprites(game->spriteRenderer, game->reg);
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
    SetExitKey(0);

    auto game = std::make_unique<Game>();

    LoadGame(game);
    LoadAllAssets();

    LoadSpriteRenderer(game->spriteRenderer);

    GotoScene(game, new MenuScene(game->reg));
//    GotoScene(game, new GameScene(game->reg));

    const std::set<ActorName> actors { ActorName::Player, ActorName::OldMan };

    Play play;
    play.script = Script{
        {
            Action{ actors, [](entt::registry& reg, const entt::entity self){
                auto& actor = reg.get<Actor>(self);
                auto& physics = reg.get<Physics>(self);
                if (actor.actorName == ActorName::Player) {
                    if (actor.timer[0] < 2.0f) {
                        physics.velocity.x = 20;
                        actor.timer[0] += GetFrameTime();
                        return false;
                    } else {
                        actor.timer[0] = 0.0f;
                        return true;
                    }
                }
                return false;
            } },

            Action{ actors, [](entt::registry& reg, const entt::entity self){
                auto& actor = reg.get<Actor>(self);
                auto& physics = reg.get<Physics>(self);
                if (actor.actorName == ActorName::Player) {
                    if (actor.timer[0] < 2.0f) {
                        physics.velocity.x = -20;
                        actor.timer[0] += GetFrameTime();
                        return false;
                    } else {
                        physics.velocity.y = -300.0f;
                        physics.on_ground = false;
                        return true;
                    }
                }
                return false;
            } },

            Action{ actors, [](entt::registry& reg, const entt::entity self){
                auto& actor = reg.get<Actor>(self);
                auto& physics = reg.get<Physics>(self);
                if (actor.actorName == ActorName::Player) {
                    if (physics.on_ground) return true;
                }
                return false;
            } },
        },
        [](entt::registry& reg, const entt::entity& self){
            auto& actor = reg.get<Actor>(self);
            actor.timer[0] = 0.0f;
            return true;
        }
    };

    while (!WindowShouldClose() && game->state != AppState::Stopped) {
        const auto screenWidth = GetScreenWidth();
        const auto screenHeight = GetScreenHeight();

        if (IsWindowFocused()) {
            if (IsKeyPressed(KEY_Y))
                DoScreenPlay(game->reg, game->stage, play);

            Update(game);
            Render(game);

            BeginDrawing();
            ClearBackground(BLACK);
            // DrawTexture(game->mainCanvas.texture, 0, 0, WHITE);
            const auto aspect = (float) CANVAS_HEIGHT / (float) CANVAS_WIDTH;

            const auto width = (float) GetScreenWidth();
            const auto height = width * aspect;
            const auto x = screenWidth / 2.0f - width / 2.0f;
            const auto y = screenHeight / 2.0f - height / 2.0f;

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
                        {0, 0, (float) GetScreenHeight() * aspect, (float) GetScreenHeight()},
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
