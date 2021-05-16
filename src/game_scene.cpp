#include "game_scene.hpp"

GameScene::GameScene(entt::registry& reg) {}

void GameScene::loadLevel(uptr<Game>& game, const std::string& which){
    game->level = LoadLevel(which);
    game->physicsPaused = true;
    loadTimer = 1.0f;
    auto* tilemap = GetTilemap(game->level);

    SpawnEntitiesFromTileMap(tilemap, game);

    float playerHeight = 64;
    game->reg.view<Player, Body, Character>().each([this, &playerHeight](auto& p, auto& body, Character& character){
        respawnLocation = Vector2{body.x, body.y};
        playerHeight = body.height;
        character.equiped.weapon = Assets::I()->getItemInfo("small-sword"); 
    });

    // Spawn checkpoints and kill zones
    for (const auto& feat : tilemap->features){
        if (feat.type == FeatureType::Checkpoint) {
            auto e = game->reg.create();
            auto& b = game->reg.emplace<Body>(e, Body{feat.x, feat.y, feat.width, feat.height});
            auto& i = game->reg.emplace<Interaction>(e);
            i.mode = InteractionMode::CALL_WHEN_ENTERED;
            i.icon = ActionIcon::NONE;
            i.action = [this, playerHeight](auto e, entt::registry& reg) {
                const auto& b = reg.get<Body>(e);
                respawnLocation = Vector2{b.x + b.width / 2, b.y + b.height - playerHeight};
            };
        }

        if (feat.type == FeatureType::Kill) {
            auto e = game->reg.create();
            auto& b = game->reg.emplace<Body>(e, Body{feat.x, feat.y, feat.width, feat.height});
            auto& i = game->reg.emplace<Interaction>(e);
            i.mode = InteractionMode::CALL_WHEN_ENTERED;
            i.icon = ActionIcon::NONE;
            i.action = [&](auto e, entt::registry& reg) {
                game->reg.view<Player, Body>().each([this](auto& p, auto& body){
                    body.x = respawnLocation.x;
                    body.y = respawnLocation.y;
                });
            };
        }

        if (feat.type == FeatureType::Banner) {
            auto e = game->reg.create();
            auto& b = game->reg.emplace<Body>(e, Body{feat.x, feat.y, feat.width, feat.height});
            auto& i = game->reg.emplace<Interaction>(e);
            i.mode = InteractionMode::CALL_WHEN_ENTERED;
            i.icon = ActionIcon::NONE;
            i.action = [&](auto e, entt::registry& reg) {
                DoAreaBanner(game, feat.target);
            }; 
        }
    }
}

void GameScene::load(uptr<Game>& game) {
    loadLevel(game, "resources/maps/StartIslandFork.tmx");
}

void GameScene::update(uptr<Game>& game) {
    handlePorts(game);
    game->physicsPaused = loadTimer > 0.0f;
    loadTimer -= GetFrameTime();

    // Handle rooms

//    auto* tilemap = GetTilemap(game->level);
//    if (tilemap) {
//        for (auto& room : tilemap->rooms) {
//            game->reg.view<Player, Body>().each([&room](auto& _player, auto& body){
//                room.active = CheckCollisionRecs(body, room);
//            });
//        }
//    }
}

void GameScene::handleDoors(uptr<Game>& game) {
}

void GameScene::handlePorts(uptr<Game>& game) {
    auto* tilemap = GetTilemap(game->level);
    const auto id = GetTilemapId(game->level);
    if (enteringPort && tilemap) {
        if (fadeOut) {
            if (fadeTimer <= 0.01f) {

                // Store a snapshot of the entities
                archiveEntities(game, id);
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

                SpawnEntitiesFromTileMap(GetTilemap(game->level), game);

                // Un archive entities for that tilemap
                unarchiveEntities(game, GetTilemapId(game->level));

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
            if (feat.type == FeatureType::Door) {
                if (Input::I()->EnterDoor()) {
                    if (CheckCollisionRecs(feat.bounds(), body)) {
                        // Phase out the tilemap

                        std::cout << "TARGET: " << feat.target << std::endl;
                        deleteLevel(game);
                        loadLevel(game, "resources/maps/" + feat.target + ".tmx");

                    }
                }
            } else if (feat.type == FeatureType::Port) {
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

                        if (const auto returnPortO = GetPortWithTarget(tmap, tilemap->name, feat.id)){
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

void GameScene::deleteLevel(uptr<Game>& game) {
    game->reg.clear();
}

void GameScene::render(const uptr<Game>& game) {
}

void GameScene::destroy(uptr<Game>& game) {
    game->reg.clear();
}

void GameScene::archiveEntities(uptr<Game>& game, int id) {
    game->reg.each([&game, id](auto entity){
        if (game->reg.has<Player>(entity)) return;
        if (!game->reg.has<Disabled>(entity))
            game->reg.emplace<Disabled>(entity, id);
    });
    archived = true;
}

void GameScene::unarchiveEntities(uptr<Game>& game, int id) {
    game->reg.each([&game, id](auto entity){
        if (game->reg.has<Disabled>(entity)) {
            auto& d = game->reg.get<Disabled>(entity);
            if (d.mapId == id) game->reg.remove<Disabled>(entity);
        }
    });
    archived = false;
}
