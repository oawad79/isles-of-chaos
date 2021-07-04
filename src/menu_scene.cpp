#include "menu_scene.hpp"
#include "game_scene.hpp"

MenuScene::MenuScene(entt::registry& reg) {

  for (size_t i = 0; i < (NUM_RGBS / 3); i++)
      palette[i] = Color{
              (unsigned char)rgbs[i * 3 + 0],
              (unsigned char)rgbs[i * 3 + 1],
              (unsigned char)rgbs[i * 3 + 2],
          255,
      };

  for (int i = 0; i < FIRE_WIDTH * FIRE_HEIGHT; i++)
      firePixels[i] = 0;

  for (int i = 0; i < FIRE_WIDTH; i++)
      firePixels[i + (FIRE_HEIGHT - 1) * FIRE_WIDTH] = 36;

  menuBg = LoadTexture("resources/textures/MainMenuBg.png");
}

void MenuScene::load(const uptr<Game>& game) {

}

void MenuScene::spreadFire(int src) {
    int rnd = (int)floor(RandFloat() * 3.0f) & 3;
    int dst = src - rnd + 1;

    int dst_f = dst - FIRE_WIDTH;
    int src_f = src;

    if (dst_f >= 0 && dst_f < FIRE_WIDTH*FIRE_HEIGHT &&
        src_f >= 0 && src_f < FIRE_WIDTH*FIRE_HEIGHT) {
        firePixels[dst - FIRE_WIDTH] = firePixels[src] - (rnd & 1);
    }
}

void MenuScene::update(const uptr<Game>& game) {
  static float timer = 0;

  if (timer > 0.001f) {
      for (int x = 0; x < FIRE_WIDTH; x++) {
          for (int y = FIRE_HEIGHT - 1; y >= 1; y--) {
              spreadFire(y * FIRE_WIDTH + x);
          }
      }
      timer = 0;
  } else {
      timer += GetFrameTime();
  }
}

void MenuScene::render(const uptr<Game>& game) {

  const auto size = MeasureText("Isles of Chaos!", 20);
  const auto size2 = MeasureText("Press enter to start", 10);

  DrawRectangle(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, {0,0,0,255});

  DrawText("Isles of Chaos!", CANVAS_WIDTH / 2 - size / 2, 10, 20, WHITE);

  for (int x = 0; x < FIRE_WIDTH; x++) {
      for (int y = 0; y < FIRE_HEIGHT; y++) {
          int i = firePixels[y * FIRE_WIDTH + x];
          Color color = palette[i];

           color.r = 1 - color.r;
           color.g = 1 - color.g;
           color.b = 1 - color.b;

          DrawRectangle(x, y, 1, 1, color);
      }
  }

  constexpr float h = 15;

  if (game->guiState.doButton(
        {CANVAS_WIDTH/2, CANVAS_HEIGHT/2 - (h + 6), 120, h}, 
        "New Game", 
        10, 
          GUI_FLAG_CENTER_X 
        | GUI_FLAG_CENTER_Y 
        | GUI_FLAG_CONTAINER_CENTER_X 
        | GUI_FLAG_CONTAINER_CENTER_Y)) {
    GotoScene(game, new GameScene(game->reg));
  }

  if (game->guiState.doButton(
        {CANVAS_WIDTH/2, CANVAS_HEIGHT/2, 120, h}, 
        "Load Game", 
        10, 
          GUI_FLAG_CENTER_X 
        | GUI_FLAG_CENTER_Y 
        | GUI_FLAG_CONTAINER_CENTER_X 
        | GUI_FLAG_CONTAINER_CENTER_Y)) {
    LoadGameState(game, "demo_save");
  }

  if (game->guiState.doButton(
        {CANVAS_WIDTH/2, CANVAS_HEIGHT/2 + h + 6, 120, h}, 
        "Exit", 
        10, 
          GUI_FLAG_CENTER_X 
        | GUI_FLAG_CENTER_Y 
        | GUI_FLAG_CONTAINER_CENTER_X 
        | GUI_FLAG_CONTAINER_CENTER_Y)) {
    CloseWindow();
    exit(EXIT_SUCCESS);
  }
}

void MenuScene::destroy(const uptr<Game>& game) {

}