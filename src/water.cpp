#include "water.hpp"

void UpdateWater(entt::registry& reg) {

}

void DrawWater(entt::registry& reg) {
  auto view = reg.view<Water, Body>();
  const auto tex = Assets::I()->textures[Textures::TEX_ENTITIES];
  const auto region = Rectangle{0, 80, 32, 16};

  view.each([&](auto& w, auto &b){

    for (int x = 0; x < (int)(b.width / 32); x++) {
      DrawTexturePro(
        tex,
        region,
        {b.x + 32 * x, b.y, 32, 16},
        {0, 0},
        0.0f,
        WHITE);
    }

    DrawRectangle(b.x, b.y+16.0f, b.width, b.height-16.0f, {91, 110, 225, 255});
  });
}
