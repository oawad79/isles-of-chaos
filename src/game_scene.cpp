#include "game_scene.hpp"

void GameScene::load(uptr<Game>& game) {
    game->level = LoadLevel("resources/maps/StartIsland1.tmx");
    const auto* tilemap = GetTilemap(game->level);

    SpawnEntitiesFromTileMap(tilemap, game);

    // Spawn checkpoints and kill zones
    for (const auto& feat : tilemap->features){
        if (feat.type == FeatureType::Checkpoint) {
            auto e = game->reg.create();
            auto& b = game->reg.emplace<Body>(e, Body{feat.x, feat.y, feat.width, feat.height});
            auto& i = game->reg.emplace<Interaction>(e);
            i.mode = InteractionMode::CALL_WHEN_ENTERED;
            i.action = [&](auto e, entt::registry& reg) {

            };
        }
    }
}

void GameScene::update(uptr<Game>& game) {
    auto* tilemap = GetTilemap(game->level);
    if (enteringPort && tilemap) {
        if (fadeOut) {
            if (fadeTimer <= 0.01f) {
                game->level->currentTilemap = nextTilemap;

                nextTilemap = "";
                fadeOut = false;

                auto* tmap = GetTilemap(game->level);
                tmap->alpha = 0.0f;
                SetPosition(tmap, nextOffset);
                return;
            }
        } else {
            if (fadeTimer > 0.99f) {
                enteringPort = false;
                fadeTimer = 1.0f;
                game->physicsPaused = false;
                nextOffset = Vector2{0,0};
                return;
            }
        }

        tilemap->alpha = fadeTimer;
        fadeTimer = lerp(fadeTimer, fadeOut ? 0.0f : 1.0f, GetFrameTime() * 10.0f);
    }

    game->reg.view<Player, Body>()
             .each([this, &tilemap, &game](auto& player, auto& body){
        if (!tilemap || enteringPort) return;
        for (const auto& feat : tilemap->features) {
            if (feat.type == FeatureType::Port) {
                if (Input::I()->EnterDoor()) {
                    if (CheckCollisionRecs(feat.bounds(), body)) {
                        // Phase out the tilemap

                        enteringPort = true;
                        fadeOut = true;
                        fadeTimer = 1.0f;
                        nextTilemap = feat.target;

                        auto* tmap
                            = GetTilemap(game->level, nextTilemap);
                        SetPosition(tmap, {0, 0});

                        if (const auto returnPortO = GetPortWithTarget(tmap, tilemap->name)){
                            const auto returnPort = returnPortO.value();
                            const auto f = feat.bounds();

                            nextOffset = Vector2{
                                f.x - returnPort.x,
                                f.y - returnPort.y
                            };

                            game->physicsPaused = true;
                        } else {
                            std::cout << "ERROR:: Cant find a port that returns to current tilemap!" << std::endl;
                            enteringPort = false;
                            fadeOut = false;
                            fadeTimer = 0.0f;
                            nextTilemap = "";
                            nextOffset = Vector2{0, 0};
                        }

                    }
                }
            }
        }
    });
}

void GameScene::render(const uptr<Game>& game) {
}

void GameScene::destroy(uptr<Game>& game) {
    game->reg.clear();
}
