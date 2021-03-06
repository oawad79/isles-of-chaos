#include "game_scene.hpp"

GameScene::GameScene(entt::registry &reg, const std::string gameLevel, const Vector2 tilemapPosition)
    : startGameLevel(gameLevel), startGamePosition(tilemapPosition) {}

void GameScene::loadLevel(const uptr<Game> &game, const std::string &which) {
  game->level = LoadLevel("resources/maps/" + startGameLevel + ".tmx");
  game->physicsPaused = true;
  loadTimer = 0.0f;
  auto *tilemap = GetTilemap(game->level);

  SetPosition(tilemap, startGamePosition);

  SpawnEntitiesFromTileMap(tilemap, game);
}

void GameScene::load(const uptr<Game> &game) {
  loadLevel(game, startGameLevel);
}

void GameScene::update(const uptr<Game> &game) {
  handlePorts(game);
  game->physicsPaused = loadTimer > 0.0f;
  loadTimer -= GetFrameTime();
}

void GameScene::handleDoors(const uptr<Game> &game) {
}

void GameScene::handlePorts(const uptr<Game> &game) {
  auto *tilemap = GetTilemap(game->level);
  const auto id = GetTilemapId(game->level);
  constexpr float FADE_SPEED{4.0f};
  if (enteringPort && tilemap) {
    if (fadeState == FadeState::Out) {
      this->loadTimer = FADE_SPEED;
      fadeTimer -= GetFrameTime() * FADE_SPEED;
      if (fadeTimer <= 0.02f) {
        fadeTimer = 0.0f;
        blackTimer = 1.0f;
        fadeState = FadeState::Hold;

        archiveEntities(game, id);
        game->level->currentTilemap = nextTilemap;
        nextTilemap = "";
        auto *tmap = GetTilemap(game->level);
        SetPosition(tmap, nextOffset);
      }
    } else if (fadeState == FadeState::Hold) {
      if (blackTimer <= 0.0f) {
        fadeState = FadeState::In;
        fadeTimer = 0.0f;

        game->physicsPaused = false;
        nextOffset = Vector2{0, 0};
        SpawnEntitiesFromTileMap(GetTilemap(game->level), game);
        unarchiveEntities(game, GetTilemapId(game->level));

        // Fix players position and velocity
        game->reg.view<Player, Body, Physics>().each([&](auto &_p, auto &body, auto &physics) {
          body.x = this->portCenter.x - body.width / 2;
          body.y = this->portCenter.y - body.height - 1;
          physics.velocity.x = 0.0f;
          physics.on_ground = true;
        });
      }
      blackTimer -= GetFrameTime() * 2;
    } else if (fadeState == FadeState::In) {
      this->loadTimer = 0.2f;
      fadeTimer += GetFrameTime() * FADE_SPEED;
      if (fadeTimer >= 0.95f) {
        fadeTimer = 1.0f;
        enteringPort = false;
      }
    }

    tilemap->alpha = 1.0f;
    game->shade = Clamp(1.0f - fadeTimer, 0.0f, 1.0f);
  }

  game->reg.view<Player, Body>()
          .each([this, &tilemap, &game](auto &player, auto &body) {
            if (!tilemap || enteringPort) return;
            for (const auto &feat : tilemap->features) {
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
                    fadeState = FadeState::Out;

                    fadeTimer = 1.0f;
                    nextTilemap = feat.target;

                    auto *tmap = GetTilemap(game->level, nextTilemap);
                    SetPosition(tmap, {0, 0});

                    if (const auto returnPortO = GetPortWithTarget(tmap, tilemap->name, feat.id)) {
                      const auto &returnPort = returnPortO.value();
                      const auto f = feat.bounds();

                      nextOffset = Vector2{
                              f.x - returnPort.x,
                              f.y - returnPort.y};

                      portCenter.x = f.x + f.width / 2;
                      portCenter.y = f.y + f.height;

                      game->physicsPaused = true;
                    } else {
                      std::cout << "ERROR:: Cant find a port that returns to current tilemap!" << std::endl;
                      enteringPort = false;
                      fadeState = FadeState::In;
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

void GameScene::deleteLevel(const uptr<Game> &game) {
  game->reg.clear();
}

void GameScene::render(const uptr<Game> &game) {
}

void GameScene::destroy(const uptr<Game> &game) {
  game->reg.clear();
}

void GameScene::archiveEntities(const uptr<Game> &game, int id) {
  game->reg.each([&game, id](auto entity) {
    if (game->reg.has<Player>(entity)) return;
    if (game->reg.has<Actor>(entity) && game->reg.get<Actor>(entity).type == ActorType::KIWI_BIRD) return;
    if (!game->reg.has<Disabled>(entity))
      game->reg.emplace<Disabled>(entity, id);
  });
  archived = true;
}

void GameScene::unarchiveEntities(const uptr<Game> &game, int id) {
  game->reg.each([&game, id](auto entity) {
    if (game->reg.has<Disabled>(entity)) {
      auto &d = game->reg.get<Disabled>(entity);
      if (d.mapId == id) game->reg.remove<Disabled>(entity);
    }
  });
  archived = false;
}
