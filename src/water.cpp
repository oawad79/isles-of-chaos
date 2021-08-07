#include "water.hpp"

void UpdateWater(Feature& water) {
  const auto& shader = Assets::I()->shaders[Shaders::WATER_SHADER];

  float timer[] = { (float)GetTime() };

  if (water.timerLoc == -1 || shader.locs[32 - 1] == 1) {
    water.timerLoc = GetShaderLocation(shader, "timer");
  }

  SetShaderValue(shader, water.timerLoc, timer, SHADER_UNIFORM_FLOAT);
}

void DrawWater(const Feature& water) {
  const auto& tex = Assets::I()->textures[Textures::TEX_ENTITIES];
  const auto& shader = Assets::I()->shaders[Shaders::WATER_SHADER];

  constexpr auto waterRes = 4.0f;
  const auto wx = water.x + water.offset.x;
  const auto wy = water.y + water.offset.y;

  const auto region = Rectangle{0, 80, waterRes, 32};

  DrawRectangle(wx, wy+16.0f, water.width, water.height-16.0f, {91, 110, 225, 255});
  BeginShaderMode(shader);
    for (int x = 0; x < (int)(water.width / waterRes); x++) {
      DrawTexturePro(
        tex,
        region,
        {floor(wx + waterRes * x), floor(wy), waterRes, 32},
        {0, 0},
        0.0f,
        WHITE);
    }
  EndShaderMode();
}
