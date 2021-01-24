#include "water.hpp"

void UpdateWater(entt::registry& reg) {

}

void DrawWater(entt::registry& reg) {
  auto view = reg.view<Water, Body>(entt::exclude<Disabled>);
  const auto tex = Assets::I()->textures[Textures::TEX_ENTITIES];
  const auto region = Rectangle{0, 80, 32, 16};

  view.each([&](auto& w, auto &b){

    // Handle splashes
    // TODO(Dustin): Actually why not use interaction to spawn splash particles?
    reg.view<Physics, Body>(entt::exclude<Particle, Water, Disabled>)
      .each([&](auto& physics, auto& body) {
      if (std::abs(physics.velocity.y) > 50.0f) {
        if (CheckCollisionRecs(b, body)){
          const float depth = body.y + body.height - b.y;
          if (depth < 20) {
            SpawnWaterParticles(reg, body.center(), 40 * std::abs(physics.velocity.y));
          }
        }
      }
    });

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
