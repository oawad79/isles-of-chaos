#include "menu_scene.hpp"

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
  for (int x = 0; x < FIRE_WIDTH; x++) {
    for (int y = FIRE_HEIGHT - 1; y >= 1; y--) {
      spreadFire(y * FIRE_WIDTH + x);
    }
  }
}

void MenuScene::render(const uptr<Game>& game) {
  // std::cout << "HERE!" << std::endl;
  // DrawRectangle(0, 0, 100, 100, RED);
  for (int x = 0; x < FIRE_WIDTH; x++) {
      for (int y = 0; y < FIRE_HEIGHT; y++) {
          int i = firePixels[y * FIRE_WIDTH + x];

          // float c = (float)i / 36.0f;
          // if (c > 1.0f) c = 1.0f;
          // if (c < 0.0f) c = 0.0f;
          // Vector4 a = COLOR_TO_VEC4(palette[i]);
          // a.z = 0.0f;

          // Vector4 b = Vector4{0.2f, 0.5f, 0.5f, c};

          // Vector4 total = Vector4{
          //   a.x + b.x,
          //   a.y + b.y,
          //   a.z + b.z,
          //   a.w + b.w
          // };

          // Color color = VEC4_TO_COLOR(total);
          Color color = palette[i];
          color.r = 255 - color.r;
          color.g = 255 - color.g;
          color.b = 255 - color.b;

          DrawRectangle(x, (FIRE_HEIGHT-y), 1, 1, color);

          DrawRectangle(x, y + FIRE_HEIGHT, 1, 1, color);

          // if (GetTime() < 20
          //     DrawRectangle(x, FIRE_HEIGHT - y, 1, 1, color);
          // } else {
          // }
      }
  }
}

void MenuScene::destroy(uptr<Game>& game) {

}