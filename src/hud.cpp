#include "hud.hpp"

void DrawHealth(Vector2 where, const uptr<Game> &game, Texture2D tex) {
  const auto full = Rectangle{32, 8, 8, 8};
  const auto fullTip = Rectangle{40, 8, 8, 8};
  const auto empty = Rectangle{48, 8, 8, 8};
  const auto emptyTip = Rectangle{56, 8, 8, 8};

  constexpr int margin = 2;
  constexpr float startX = 0;

  const float healthBarX = where.x + 16 + margin;

  static float lastAmount = -1;

  game->reg.view<Player, Health>().each([&](auto &p, auto &health) {
    if (lastAmount < 0) {
      lastAmount = health.amount;
    }

    lastAmount = Lerp(lastAmount, health.amount, GetFrameTime()*4.0f);

    DrawTexturePro(tex, {0, 96, 16, 16}, {where.x, where.y, 16, 16}, {0, 0}, 0.0f, WHITE);

    const int length = (int)(health.max/8.0f);
    const float healthLength = (int)(health.amount/8.0f);

    const float percent = (float) health.max / (float) health.amount;

    auto reg = full;
    reg.x -= 8;
    DrawTexturePro(tex, reg, {healthBarX - 8, where.y, 8, 8}, {0, 0}, 0.0f, {169, 59, 59, 255});

    const auto totalWidth = 8 * (float) length;
    const auto dx = totalWidth - (totalWidth * percent);

    //BeginScissorMode(healthBarX - 1, where.y, totalWidth + 2, 8);
    const auto color = Color{169, 59, 59, 255};

    for (int i = 0; i < length; i++) {
      auto reg = i == length - 1 ? fullTip : full;
      const float x = healthBarX + 8 * (float)i;
      reg.x += 16; DrawTextureRec(tex, reg, Vector2{x, where.y}, color);
    }


    DrawRectangle(healthBarX, where.y+1, lastAmount-8, 3, WHITE);
    DrawRectangle(healthBarX+lastAmount - 8, where.y+1, 1, 2, WHITE);
    DrawRectangle(healthBarX+lastAmount - 7, where.y+1, 1, 1, WHITE);

    DrawRectangle(healthBarX, where.y+1, health.amount-8, 3, color);
    DrawRectangle(healthBarX+health.amount - 8, where.y+1, 1, 2, color);
    DrawRectangle(healthBarX+health.amount - 7, where.y+1, 1, 1, color);

    //EndScissorMode();
  });
}

// The arrow buttons
void DrawQuickButtonControls(Vector2 where, const uptr<Game> &game, Texture2D tex) {
  const auto thumbPadRegion = Rectangle{0, 496, 16, 16};
  DrawTexturePro(tex, thumbPadRegion, {where.x, where.y + GUI_CANVAS_HEIGHT-16, 16, 16}, Vector2Zero(), 0.0f, WHITE);
}

void UpdateHud(const uptr<Game> &game) {
  auto& state = game->guiState;

  if (IsKeyPressed(KEY_Q)) {
    state.showHud();
  }

  game->reg.view<Player, Health>().each([&](auto &p, auto &health) {
    const auto d = Input::I()->GetMovementVector();
    state.frameTarget = d.x * 2.0f;
    state.frameScaler = lerp(state.frameScaler, state.frameTarget, GetFrameTime() * state.speed);

    const float sign = state.frameScaler < 0 ? -1 : 1;
    const float f = sign < 0 ? -state.frameScaler : state.frameScaler;
    if (f < 0.1) state.frameScaler = 0.0f;
    if (f > 1.8 && sign > 1) state.frameScaler = 2.0f;
  });

  if (state.hudHideTimer > 0.0f) {
    state.hudMargin = Lerp(state.hudMargin, HUD_MARGIN, GetFrameTime()*8.0f);
  } else {
    state.hudMargin = Lerp(state.hudMargin, HUD_HIDE, GetFrameTime()*2.0f);
  }

  state.hudHideTimer -= GetFrameTime();
}

void DrawHud(const uptr<Game> &game) {
  const auto& state = game->guiState;
  const auto tex = Assets::I()->textures[Textures::TEX_GUI];
  const float m = floor(state.hudMargin);
  DrawHealth(Vector2{m, m}, game, tex);
  DrawQuickButtonControls(Vector2{m, -m}, game, tex);
}
