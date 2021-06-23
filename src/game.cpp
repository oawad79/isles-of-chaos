#include "game.hpp"

void LoadGame(uptr<Game>& game) {
    game->mainCanvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
    game->guiCanvas = LoadRenderTexture(GUI_CANVAS_WIDTH, GUI_CANVAS_HEIGHT);

    game->mainCamera.rotation = 0;
    game->mainCamera.zoom = 1;
}

bool SaveGameState(uptr<Game>& game, const std::string& name) {
  auto saveDirPath = std::filesystem::current_path().append("/ioc-saves");
  if (!std::filesystem::exists(saveDirPath)) {
    if (!std::filesystem::create_directory(saveDirPath)) {
      std::cout << "Failed to create saves directory (ABORTING)" << std::endl;
      return false;
    }
  }
  const auto saveFilePath = saveDirPath.append("/save_" + name + ".sky-save");

  return false;
}

bool LoadGameState(uptr<Game>& game, const std::string& name) {

  return false;
}

void PushScene(uptr<Game>& game, SceneLayer* scene) {
    scene->load(game);
    game->scenes.emplace_back(scene);
}

void PopScene(uptr<Game>& game) {
    if (game->scenes.size() > 0) {
        game->scenes[game->scenes.size() - 1]->destroy(game);
        game->scenes.pop_back();
    }
}

void GotoScene(uptr<Game>& game, SceneLayer* scene) {
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
      Image screenImage = GetScreenData();
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

Vector2 MouseGuiCanvasPosition(const uptr<Game>& game) {
    const float mx = GetMouseX();
    const float my = GetMouseY();

    const float aspect = (float)GUI_CANVAS_HEIGHT / (float)GUI_CANVAS_WIDTH;
    const float width = GetScreenWidth();
    const float height = GetScreenHeight();

    const float ws = (float)GUI_CANVAS_WIDTH / (float)width;
    const float hs = (float)GUI_CANVAS_HEIGHT / (float)height;

    return {mx*ws, my*hs};
}