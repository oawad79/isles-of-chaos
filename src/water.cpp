#include "water.hpp"

void UpdateWater(entt::registry& reg) {

}

void DrawWater(entt::registry& reg) {
  auto view = reg.view<Water, Body>();

  view.each([&](auto& w, auto &b){
    DrawRectangle(b.x, b.y, b.width, b.height, {0, 0, 255, 200});
  });
}
