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

void MenuScene::load(uptr<Game>& game) {

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

void MenuScene::update(uptr<Game>& game) {
  static float timer = 0;

  if (IsKeyPressed(KEY_ENTER)) {
    GotoScene(game, new GameScene(game->reg));
  }

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
//  DrawTexturePro(this->menuBg, Rectangle{0, 0, 256, 144}, Rectangle{0, 0, 256, 144}, Vector2{0, 0}, 0.0f, WHITE);

  const auto size = MeasureText("Isles of Chaos!", 20);
  DrawText("Isles of Chaos!", CANVAS_WIDTH / 2 - size / 2, 10, 20, WHITE);

  for (int x = 0; x < FIRE_WIDTH; x++) {
      for (int y = 0; y < FIRE_HEIGHT; y++) {
          int i = firePixels[y * FIRE_WIDTH + x];
          Color color = palette[i];

//            color.r = color.r;
//            color.g = color.g;
//            color.b = color.b;

          DrawRectangle(x, y, 1, 1, color);
      }
  }

  const auto size2 = MeasureText("Press enter to start", 10);
  DrawRectangle((CANVAS_WIDTH/2-size2/2) - 10, CANVAS_HEIGHT/2-20, size2 + 20, 30, {0,0,0,100});
  DrawText("Press enter to start", CANVAS_WIDTH/2 - size2/2, CANVAS_HEIGHT/2-10, 10, ORANGE);
}

void MenuScene::destroy(uptr<Game>& game) {

}