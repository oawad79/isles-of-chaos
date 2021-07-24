#include "game.hpp"
#include "game_scene.hpp"
#include "playwright.hpp"

void LoadGame(uptr<Game>& game) {
    game->mainCanvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
    game->guiCanvas = LoadRenderTexture(GUI_CANVAS_WIDTH, GUI_CANVAS_HEIGHT);

    game->mainCamera.rotation = 0;
    game->mainCamera.zoom = 1;
}

std::filesystem::path GetSavePath(const std::string name) {
  auto saveDirPath = std::filesystem::current_path().append("ioc-saves");
  if (!std::filesystem::exists(saveDirPath)) {
    if (!std::filesystem::create_directory(saveDirPath)) {
      std::cout << "Failed to create saves directory (ABORTING)" << std::endl;
      return saveDirPath / ("save_" + name + ".sky-save");
    }
  }

  return saveDirPath / ("save_" + name + ".sky-save");
}

bool SaveGameState(const uptr<Game>& game, const std::string& name) {
  using namespace tinyxml2;

  const auto saveFilePath = GetSavePath(name);

  std::cout << "Creating save: => " << saveFilePath << std::endl;

  XMLDocument doc;
  auto* root = doc.NewElement("save");
  root->SetAttribute("name", name.c_str());
  doc.InsertFirstChild(root);

  auto* worldGroupFirst = doc.NewElement("world");
  auto* worldGroup = root->InsertFirstChild(worldGroupFirst);

  auto* entityGroupFirst = doc.NewElement("entities");
  auto* entityGroup = root->InsertFirstChild(entityGroupFirst);

  if (game->scenes.size() > 0) {
    auto* s = game->scenes[0];
    if (dynamic_cast<GameScene*>(s) != nullptr) {
      auto* gs = dynamic_cast<GameScene*>(s);

      // TODO(Save game state stuff)

    } else {
      std::cout << "Save can't find game state (ABORTING)" << std::endl;
      return false;
    }
  } else {
    std::cout << "Save can't find game state (ABORTING)" << std::endl;
    return false;
  }

  {
    for (const auto* tilemap : game->level->tilemaps) {
      auto* tm = doc.NewElement("loaded-tilemap");
      tm->SetAttribute("name", tilemap->name.c_str());
      tm->SetAttribute("path", tilemap->path.c_str());
      tm->SetAttribute("position-updated", tilemap->positionUpdated);
      worldGroup->InsertFirstChild(tm);
    }

    worldGroupFirst->SetAttribute("current", game->level->currentTilemap.c_str());

    {
      auto* tilemap = GetTilemap(game->level);
      worldGroupFirst->SetAttribute("position.x", tilemap->position.x);
      worldGroupFirst->SetAttribute("position.y", tilemap->position.y);
    }
  }

  {
    game->reg.each([&](auto entity) {
      auto* entityNode = doc.NewElement("entity");

      if (game->reg.has<Ent>(entity)) {
        auto& ent = game->reg.get<Ent>(entity);
        entityNode->SetAttribute("name", EntTypeS[(int)ent.entType].c_str());
        entityNode->SetAttribute("entType", (int)ent.entType);
        entityGroup->InsertFirstChild(entityNode);
      } else {
//        std::cout << "Required component Ent is not found on entity" << std::endl;
        return;
      }

      if (game->reg.has<Disabled>(entity)) {
        entityNode->SetAttribute("disabled", true);
      }

      if (game->reg.has<Body>(entity)) {
        auto &body = game->reg.get<Body>(entity);

        auto* bodyNode = doc.NewElement("c");
        entityNode->InsertFirstChild(bodyNode);

        bodyNode->SetAttribute("typeName", "Body");
        bodyNode->SetAttribute("x", body.x);
        bodyNode->SetAttribute("y", body.y);
        bodyNode->SetAttribute("width", body.width);
        bodyNode->SetAttribute("height", body.height);
      }

      if (game->reg.has<Physics>(entity))  {
        auto &physics = game->reg.get<Physics>(entity);

        auto* physicsNode = doc.NewElement("c");
        entityNode->InsertFirstChild(physicsNode);

        physicsNode->SetAttribute("typeName", "Physics");
        physicsNode->SetAttribute("type", (int)physics.type);
        physicsNode->SetAttribute("velocity.x", physics.velocity.x);
        physicsNode->SetAttribute("velocity.y", physics.velocity.y);
        physicsNode->SetAttribute("friction", physics.friction);
        physicsNode->SetAttribute("gravityScale.x", physics.gravityScale.x);
        physicsNode->SetAttribute("gravityScale.y", physics.gravityScale.y);
        physicsNode->SetAttribute("on_ground", physics.on_ground);
        physicsNode->SetAttribute("on_ladder", physics.on_ladder);
        physicsNode->SetAttribute("on_ground_timer", physics.on_ground_timer);
        physicsNode->SetAttribute("colliding_with_solid", physics.colliding_with_solid);
        //...
        physicsNode->SetAttribute("facingX", physics.facingX);
      }
    });
  }

  doc.SaveFile(saveFilePath.string().c_str());

  return false;
}

bool LoadGameState(const uptr<Game>& game, const std::string& name) {
  using namespace tinyxml2;

  const auto saveFilePath = GetSavePath(name);

  XMLDocument doc;
  doc.LoadFile(saveFilePath.string().c_str());

  auto* root = doc.FirstChildElement("save");

  auto* world = root->FirstChildElement("world");

  Vector2 tilemapPosition = Vector2{
    world->FloatAttribute("position.x", 0.0f),
    world->FloatAttribute("position.y", 0.0f)
  };

  const auto* currentTilemap = world->Attribute("current");

  auto* entities = root->FirstChildElement("entities");
  XMLElement* entity = entities->FirstChildElement("entity");

  while (entity) {
    EntType entType = static_cast<EntType>(entity->IntAttribute("entType"));

    switch (entType) {
      case EntType::Player: {
        auto pos = Vector2Zero();

        auto *comp = entity->FirstChildElement("c");
        while (comp){

          if (strcmp(comp->Attribute("typeName"), "Body") == 0) {
            pos.x = comp->FloatAttribute("x", 0.0f);
            pos.y = comp->FloatAttribute("y", 0.0f);
          }

          comp = comp->NextSiblingElement("c");
        }

        auto self = Spawn(entType, game, pos);
//        game->reg.get<Physics>(self).gravityScale.y = 0.0f  ;

        break;
      }
    }

    entity = entity->NextSiblingElement("entity");
  }

  GotoScene(game, new GameScene(game->reg, currentTilemap, tilemapPosition));

//  auto* tilemap = GetTilemap(game->level);
//  tilemap->position = tilemapPosition;

  return false;
}

void PushScene(const uptr<Game>& game, SceneLayer* scene) {
    scene->load(game);
    game->scenes.emplace_back(scene);
}

void PopScene(const uptr<Game>& game) {
    if (game->scenes.size() > 0) {
        game->scenes[game->scenes.size() - 1]->destroy(game);
        game->scenes.pop_back();
    }
}

void GotoScene(const uptr<Game>& game, SceneLayer* scene) {
    PopScene(game);
    PushScene(game, scene);
}

void UpdateGame(uptr<Game>& game) {
  if (IsKeyPressed(KEY_P)) {
    SaveGameState(game, "demo_save");
  }

  constexpr int delay = -0.1f;

  if (IsKeyPressed(KEY_F1)) {
    if (game->recordingGif) {
      std::cout << "Ending Recording" << std::endl;
      const auto fileName = "recording.gif";
      GifBegin(&game->gifWriter, fileName, GIF_WIDTH, GIF_HEIGHT, delay);
      for (int i = 0; i < game->totalFrames.size(); i+=1) {
        auto& buffer = game->totalFrames[i];
        GifWriteFrame(&game->gifWriter, buffer.data(), GIF_WIDTH, GIF_HEIGHT, 0.0f);
      }
      GifEnd(&game->gifWriter);
      game->totalFrames.clear();
    } else {
      game->recordingGif = true;
//      game->gifWriter = GifWriter();
    }
  }

  if (game->recordingGif) {
//    if (game->frameTimer >= 0.1f) {
      Image screenImage = LoadImageFromScreen();
      ImageResizeNN(&screenImage, GIF_WIDTH, GIF_HEIGHT);
      std::vector<uint8_t> buffer;
      buffer.resize(GIF_WIDTH * GIF_HEIGHT * 4);
      for (int i = 0; i < screenImage.width * screenImage.height * 4; i++) {
        buffer[i] = ((uint8_t *) screenImage.data)[i];
      }
      game->totalFrames.push_back(buffer);
      UnloadImage(screenImage);
//    } else {
//      game->frameTimer += GetFrameTime();
//    }
  }

  if (game->state == AppState::Running) {
    UpdatePlaywright(game->stage, game->reg);
    UpdateSprites(game->reg);
    UpdatePlayer(game, game->reg);
    UpdatePhysics(game, game->reg);
    UpdateTimed(game->reg);
    UpdateCharacter(game->reg);
    UpdateActor(game->reg);
    UpdateInteraction(game, game->reg);
  } else if (game->state == AppState::PauseMenu) {
  }

  for (auto* scene : game->scenes)
    scene->update(game);

}

void RenderGame(const uptr<Game>& game) {
    for (auto* scene : game->scenes)
        scene->render(game);
}

void DoDialog(const uptr<Game>& game, const DialogTree& tree) {
    if (tree.branches.find("start") == tree.branches.end()) {
        std::cout << "Error: dialog doesn't have a 'start' branch" << std::endl;
    }

    game->dialogTree = std::optional<DialogTree>(tree);
    if (game->state != AppState::InDialog)
      game->lastState = game->state;
    game->state = AppState::InDialog;
}

Vector2 MouseCanvasPosition(const uptr<Game>& game) {
    const float mx = GetMouseX();
    const float my = GetMouseY();

    const float aspect = (float)CANVAS_HEIGHT / (float)CANVAS_WIDTH;
    const float width = GetScreenWidth();
    const float height = GetScreenHeight();

    const float ws = (float)CANVAS_WIDTH / (float)width;
    const float hs = (float)CANVAS_HEIGHT / (float)height;

    return {mx*ws, my*hs};
}
