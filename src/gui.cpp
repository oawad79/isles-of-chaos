#include "gui.hpp"

constexpr auto cellSize{16};
constexpr auto margin{1};

constexpr auto dialogWidth = GUI_CANVAS_WIDTH - (GUI_CANVAS_WIDTH / 4);
constexpr auto dialogHeight = GUI_CANVAS_HEIGHT / 3;

const auto dialogX = GUI_CANVAS_WIDTH / 2 - dialogWidth / 2;
const auto dialogY = GUI_CANVAS_HEIGHT - (dialogHeight + 16);

void UpdateHud(const uptr<Game> &game, GuiState &state) {
  game->reg.view<Player, Health>().each([&](auto &p, auto &health) {
    const auto d = Input::I()->GetMovementVector();
    state.frameTarget = d.x * 2.0f;
    state.frameScaler = lerp(state.frameScaler, state.frameTarget, GetFrameTime() * state.speed);

    const float sign = state.frameScaler < 0 ? -1 : 1;
    const float f = sign < 0 ? -state.frameScaler : state.frameScaler;
    if (f < 0.1) state.frameScaler = 0.0f;
    if (f > 1.8 && sign > 1) state.frameScaler = 2.0f;
  });
}

void DrawHud(const uptr<Game> &game, GuiState &state, const Texture &tex) {
  const auto full = Rectangle{32, 8, 8, 8};
  const auto fullTip = Rectangle{40, 8, 8, 8};
  const auto empty = Rectangle{48, 8, 8, 8};
  const auto emptyTip = Rectangle{56, 8, 8, 8};

  game->reg.view<Player, Health>().each([&](auto &p, auto &health) {
    const auto &tex = Assets::I()->textures[TEX_GUI];

    DrawTexturePro(tex, {0, 96, 16, 16}, {2, 2, 16, 16}, {0, 0}, 0.0f, WHITE);

    const float percent = (float) health.max / (float) health.amount;

    constexpr int length = 8;
    constexpr float startX = 2 + 16 + 2;

    auto reg = full;
    reg.x -= 8;
    DrawTexturePro(tex, reg, {startX - 8, 2, 8, 8}, {0, 0}, 0.0f, {169, 59, 59, 255});

    const auto totalWidth = 8 * (float) length;
    const auto dx = totalWidth - (totalWidth * percent);

    BeginScissorMode(startX, 2, totalWidth, 8);
    for (int i = 0; i < length; i++) {

      auto reg = i == length - 1 ? fullTip : full;

      reg.x += 16;
      DrawTexturePro(tex, reg, {startX + 8 * (float) i, 2, 8, 8}, {0, 0}, 0.0f, {169, 59, 59, 255});

      reg.x -= 16;
      DrawTexturePro(tex, reg, {startX + (8 * (float) i) + dx, 2, 8, 8}, {0, 0}, 0.0f, {169, 59, 59, 255});
    }
    EndScissorMode();

    //int frame = floor(state.frameScaler);
    //if (state.frameScaler > 1.8) frame = 2;
    //const auto [rx, ry, rw, rh] = state.healthRegion;

    //        const float dist = health.max - health.amount;
    //        DrawRectangle(0, dist, 16, health.max - dist, RED);


    //DrawTexturePro(
    //    tex,
    //    {rx + frame * 32,
    //     ry,
    //     rw,
    //     rh},
    //    {8, 8, 32, 16},
    //    {0, 0},
    //    0.0f,
    //    WHITE);
  });
}


struct InvSpacial {
  Vector2 startPos;
  Vector2 equipStartPos;
  Vector2 totalSize;
  Vector2 weaponPos;
};

void DrawPauseMenu(const uptr<Game> &uniquePtr);

InvSpacial GetInvSpacial(Inventory &inv) {
  const float width = inv.maxColumns * (cellSize + margin);
  const float height = inv.maxRows * (cellSize + margin);
  constexpr auto cwh = GUI_CANVAS_WIDTH / 2;
  constexpr auto chh = GUI_CANVAS_HEIGHT * (0.75);

  const auto scx = cwh - width / 2;
  const auto scy = chh - height / 2;
  float cx = scx;
  float cy = scy;

  float ex = cx - cellSize - margin * 4;
  float ey = cy;

  float wx = ex - cellSize - margin;
  float wy = ey + cellSize + margin;

  return {{cx, cy},
          {ex, ey},
          {width, height},
          {wx, wy}};
}

constexpr auto fntSize = 10.0f;
constexpr auto spacing = 1.0f;

void DrawInventory(const uptr<Game> &game, GuiState &state) {
  auto drawItem = [&state](float x, float y, float size, Texture2D &tex, Item &item) {
    const auto pos = Vector2{
            x + cellSize / 2 - item.region.width / 2,
            y + cellSize / 2 - item.region.height / 2,
    };

    DrawTexturePro(
            tex,
            item.region,
            {pos.x, pos.y, item.region.width, item.region.height},
            {0, 0},
            0.0,
            WHITE);
  };

  auto drawEquipmentSquare = [&drawItem](float x, float y, std::optional<Item> o, Color border = BLACK) {
    DrawRectangle(x, y, cellSize, cellSize, {0, 0, 0, 200});
    DrawRectangleLines(x, y, cellSize, cellSize, border);
    if (o) {
      auto tex = Assets::I()->textures[Textures::TEX_EQUIPMENT];
      if (o->catagory == ItemCatagory::Ability)
        tex = Assets::I()->textures[Textures::TEX_ITEMS];
      auto item = o.value();
      drawItem(x, y, cellSize, tex, item);
    }
  };

  auto drawTooltip = [&](std::optional<Item> o, float cx, float cy) {
    if (!o) return;

    if (CheckCollisionPointRec(GetMouseGuiPosition(), {cx, cy, cellSize, cellSize})) {
      // Show tooltip
      auto item = o.value();

      const char *name = TextFormat("Name: %s", item.name.c_str());
      const char *catagory = TextFormat("Type: %s", ItemCatagoryS[(int) item.catagory].c_str());
      const char *value = TextFormat("Value: %d$", item.value);

      const auto nameH = MeasureTextEx(GetFontDefault(), name, fntSize, spacing).y;
      const auto categoryH = MeasureTextEx(GetFontDefault(), catagory, fntSize, spacing).y;
      const auto effectH = MeasureTextEx(GetFontDefault(), "Effect", fntSize, spacing).y;
      const auto valueH = MeasureTextEx(GetFontDefault(), value, fntSize, spacing).y;
      auto totalHeight = nameH + categoryH + valueH + effectH;

      const auto toolTipX = cx + 32.0f;
      auto toolTipY = cy;

      const auto xx = toolTipX;
      auto yy = toolTipY + 2 - totalHeight / 2.0f;

      DrawRectangle(
              toolTipX, toolTipY - totalHeight / 2.0f,
              GUI_CANVAS_WIDTH,
              totalHeight + 4,
              {0, 0, 0, 240});

      DrawTextEx(GetFontDefault(), name, {xx + 4, yy}, fntSize, spacing, WHITE);
      yy += nameH;
      DrawTextEx(GetFontDefault(), catagory, {xx + 4, yy}, fntSize, spacing, WHITE);
      yy += categoryH;

      switch (item.catagory) {
        case ItemCatagory::Consumable: {
          const char *effect = TextFormat("Effect: %s", ConsumableEffectS[(int) item.consumableEffect].c_str());
          DrawTextEx(GetFontDefault(), effect, {xx + 4, yy}, fntSize, spacing, WHITE);
          yy += MeasureTextEx(GetFontDefault(), effect, fntSize, spacing).y;
          break;
        }
        case ItemCatagory::Weapon: {
          const char *damage = TextFormat("Damage: %f", item.effectValue);
          DrawTextEx(GetFontDefault(), damage, {xx + 4, yy}, fntSize, spacing, WHITE);
          yy += MeasureTextEx(GetFontDefault(), damage, fntSize, spacing).y;
          break;
        }
        case ItemCatagory::Armor: {
          const char *armor = TextFormat("Armor: %f", item.effectValue);
          DrawTextEx(GetFontDefault(), armor, {xx + 4, yy}, fntSize, spacing, WHITE);
          yy += MeasureTextEx(GetFontDefault(), armor, fntSize, spacing).y;
          break;
        }
        default:
          break;
      }

      DrawTextEx(GetFontDefault(), value, {xx + 4, yy}, fntSize, spacing, WHITE);
    }
  };

  const auto consu = Assets::I()->textures[Textures::TEX_ITEMS];
  const auto equip = Assets::I()->textures[Textures::TEX_EQUIPMENT];

  const auto drawSlot = [&](Slot slot, float cx, float cy) {
    const int rx = cx + slot.column * (cellSize + margin);
    const int ry = cy + slot.row * (cellSize + margin);
    DrawRectangle(rx, ry, cellSize, cellSize, {0, 0, 0, 200});

    auto color = BLACK;
    if (slot.column < 0.0f) { color = GOLD; }
    DrawRectangleLines(rx, ry, cellSize, cellSize, color);

    if (CheckCollisionPointRec(GetMouseGuiPosition(), {(float) rx, (float) ry, cellSize, cellSize})) {
      DrawRectangleLines(rx, ry, cellSize, cellSize, WHITE);
    }

    if (slot.it) {
      Item o = slot.it.value();
      auto tex = (o.catagory == ItemCatagory::Consumable || o.catagory == ItemCatagory::Money || o.catagory == ItemCatagory::Ability) ? consu : equip;
      drawItem(rx, ry, cellSize, tex, o);

      // Amount display
      const char *text = FormatText("%d", o.amount);
      DrawRectangle(cx + cellSize - 8, cy + cellSize - 5, 8, 8, WHITE);
      DrawTextEx(GetFontDefault(), text, {cx + cellSize - 6, cy + cellSize - 5}, 8.0f, 8.0f, BLACK);

      drawTooltip(o, cx, cy);
    }
  };

  game->reg
          .view<Player, Inventory, Character>()
          .each([&](auto &player, Inventory &inventory, Character &character) {
            if (state.playerInvOpen) {
              const auto &equiped = character.equiped;

              const auto sp = GetInvSpacial(inventory);
              auto [ex, ey] = sp.equipStartPos;
              const auto [wx, wy] = sp.weaponPos;

              const auto consu = Assets::I()->textures[Textures::TEX_ITEMS];
              const auto equip = Assets::I()->textures[Textures::TEX_EQUIPMENT];

              auto [cx, cy] = sp.startPos;

              for (const auto &slot : inventory.slots) drawSlot(slot, cx, cy);
              for (const auto &slot : equiped.slots) drawSlot(slot, cx, cy);
            }
          });
}

void UpdateDialog(DialogTree &dialog, const uptr<Game> &game) {
  auto &branch = dialog.branches[dialog.currentId];
  if (IsKeyPressed(KEY_A)) branch.cursor -= 1;
  if (IsKeyPressed(KEY_D)) branch.cursor += 1;
  branch.cursor %= (dialog.branches.size() + 1);

  if (Input::I()->Interact()) {
    if (branch.choices.size() == 0) {
      if (branch.nextId == "" || branch.nextId == "exit") {
        game->state = AppState::Running;
        game->dialogTree = std::nullopt;
      }
    } else {
      dialog.currentId = branch.choices[branch.cursor].nextId;
      if (dialog.currentId == "" || dialog.currentId == "exit") {
        game->state = AppState::Running;
        game->dialogTree = std::nullopt;
      }
    }
  }
}

void ShowPauseMenu(const uptr<Game> &game) {
  const auto tmp = game->state;
  game->state = game->lastState;
  game->lastState = tmp;
}

void ClosePauseMenu(const uptr<Game> &game) {
  const auto temp = game->lastState;
  game->state = game->lastState;
  game->lastState = temp;
}

void UpdatePauseMenu(const uptr<Game> &game) {
  if (IsKeyPressed(KEY_ESCAPE)) {
    ShowPauseMenu(game);
  }
}

void UpdateGui(const uptr<Game> &game) {
  if (IsKeyPressed(KEY_ESCAPE)) {
    if (game->state != AppState::PauseMenu) {
      game->lastState = game->state;
      game->state = AppState::PauseMenu;
      return;
    }
  }

  if (game->state == AppState::PauseMenu) {
    UpdatePauseMenu(game);
    return;// Important (Dont want to update dialog and stuff)
  }

  auto &state = game->guiState;

  if (state.banner.timeLeft >= 0.0f) {
    if (state.banner.color.a < 256 - 4) {
      state.banner.color.a += 4;
    } else {
      state.banner.color.a = 255;
    }

    state.banner.timeLeft -= GetFrameTime();

    if (state.banner.timeLeft <= 0.0f)
      state.banner.text = "";
  }

  UpdateHud(game, state);

  if (game->state == AppState::InDialog && game->dialogTree.has_value()) {
    auto &dialog = game->dialogTree.value();
    UpdateDialog(dialog, game);
  }

  game->reg
          .view<Player, Body, Inventory, Character>()
          .each([&state, &game](auto &player, auto &body, Inventory &inventory, Character &character) {
            if (Input::I()->OpenInv()) {
              state.playerInvOpen = !state.playerInvOpen;
              game->state = state.playerInvOpen ? AppState::InDialog : AppState::Running;
            }

            if (state.playerInvOpen) {
              const auto sp = GetInvSpacial(inventory);

              const auto [cx, cy] = sp.startPos;

              for (auto &slot : inventory.slots) {
                const int rx = cx + slot.column * (cellSize + margin);
                const int ry = cy + slot.row * (cellSize + margin);

                const auto hot = CheckCollisionPointRec(GetMouseGuiPosition(), {rx,
                                                                                ry,
                                                                                cellSize,
                                                                                cellSize});
                if (!slot.it) continue;
              }

              for (auto &slot : character.equiped.slots) {
                const int rx = cx + slot.column * (cellSize + margin);
                const int ry = cy + slot.row * (cellSize + margin);

                const auto hot = CheckCollisionPointRec(GetMouseGuiPosition(), {rx,
                                                                                ry,
                                                                                cellSize,
                                                                                cellSize});
                if (!slot.it) continue;

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inventory.isFull()) {
                  inventory.putItem(slot.it.value());
                  slot.it = std::nullopt;
                }
              }

              for (auto &slot : inventory.slots) {
                const int rx = cx + slot.column * (cellSize + margin);
                const int ry = cy + slot.row * (cellSize + margin);

                const auto hot = CheckCollisionPointRec(GetMouseGuiPosition(), {rx,
                                                                                ry,
                                                                                cellSize,
                                                                                cellSize});
                if (!slot.it) continue;
                if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && character.equip(slot.it.value())) {
                  slot.it = std::nullopt;
                }
              }
            }
          });
}

void DrawChoices(const std::vector<Choice> &choices, const int cursor) {
  const auto num = choices.size();
  const auto width = (dialogWidth - (margin * num)) / num;
  const auto font = GetFontDefault();

  int i = 0;
  for (const auto &[message, nextId] : choices) {
    const auto hot = i == cursor;
    const auto fillColor = hot ? BLACK : RAYWHITE;
    const auto rect = Rectangle{margin + dialogX + (float) i * (width + margin), dialogY + dialogHeight - 20, (float) width, 20};

    if (hot) {
      DrawRectangleRec(rect, RAYWHITE);
    }

    DrawTextRec(
            font,
            message.c_str(),
            rect,
            10,
            1.0f,
            true,
            fillColor);
    i += 1;
  }
}

void DrawBranch(const DialogBranch &branch) {
  const auto font = GetFontDefault();
  DrawTextRec(
          font,
          branch.message.c_str(),
          {dialogX + margin, dialogY + margin, dialogWidth - margin * 2, dialogHeight - margin * 2},
          10, 1.0f, true, RAYWHITE);
  if (branch.choices.size() > 0)
    DrawChoices(branch.choices, branch.cursor);
}

void DrawDialog(DialogTree &dialog) {
  // Draw border
  DrawRectangle(
          GUI_CANVAS_WIDTH / 2 - dialogWidth / 2,
          GUI_CANVAS_HEIGHT - (dialogHeight + 16),
          dialogWidth,
          dialogHeight,
          {0, 0, 0, 200});

  const auto branch = dialog.branches[dialog.currentId];
  DrawBranch(branch);
}


void DrawBanner(BannerState &state) {
  constexpr auto fontSize = 100.0f;

  const auto &tex = Assets::I()->textures[TEX_GUI];
  const auto font = GetFontDefault();
  const auto [tw, th] = MeasureTextEx(font, state.text.c_str(), fontSize, 1.0f);

  const auto x = GetScreenWidth() / 2 - tw / 2;
  const auto y = GetScreenHeight() / 2 - th / 2 - GetScreenHeight() / 4.0f;

  const auto tileSize = 8 * 6.0f;

  DrawTexturePro(tex, {0, 45, 8, 38}, {x - tileSize, y, tileSize, th}, {0, 0}, 0.0f, state.color);
  for (int i = 0; i < (int) (tw / tileSize); i++) {
    DrawTexturePro(tex, {8, 45, 16, 38}, {x + i * tileSize, y, tileSize, th}, {0, 0}, 0.0f, state.color);
  }
  DrawTexturePro(tex, {0, 45, -8, 38}, {x + tw - 4, y, tileSize + 4, th}, {0, 0}, 0.0f, state.color);

  const Color textColor{0, 0, 0, state.color.a};
  DrawTextRec(
          font,
          state.text.c_str(),
          {x, y, tw * 2, th},
          fontSize, 1.0f, true, textColor);
}

void DrawPauseMenu(const uptr<Game> &game, GuiState &guiState) {
  DrawRectangle(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, Color{0, 0, 0, 200});

  constexpr float buttonWidth{180};
  constexpr float buttonHeight{12};
  constexpr float startX{GUI_CANVAS_WIDTH / 2.50f};
  constexpr float margin{6.0f};

  game->frameTimer += GetFrameTime() * 0.65f;
  if (game->frameTimer > 1.0f)
    game->frameTimer = 1.0f;

  int i = 1;
  int diff = 12;

  float x = startX;
  if (guiState.doButton({Lerp(x + buttonWidth, x, BounceOut(game->frameTimer)), 10, buttonWidth, buttonHeight}, "Resume!", 10)) {
    ClosePauseMenu(game);
  }

  x = startX + diff * i;
  if (guiState.doButton({Lerp(x + buttonWidth, x, BounceOut(game->frameTimer)), 10 + (i++ * (buttonHeight + margin)), buttonWidth, buttonHeight}, "Save", 10, GUI_FLAG_CENTER_Y)) {
    SaveGameState(game, "demo_save");
    ClosePauseMenu(game);
  }

  x = startX + diff * i;
  if (guiState.doButton({Lerp(x + buttonWidth, x, BounceOut(game->frameTimer)), 10 + (i++ * (buttonHeight + margin)), buttonWidth, buttonHeight}, "Load", 10, GUI_FLAG_CENTER_Y)) {
    LoadGameState(game, "demo_save");
    ClosePauseMenu(game);
  }

  x = startX + diff * i;
  if (guiState.doButton({Lerp(x + buttonWidth, x, BounceOut(game->frameTimer)), 10 + (i++ * (buttonHeight + margin)), buttonWidth, buttonHeight}, "Settings", 10, GUI_FLAG_CENTER_Y)) {
    ClosePauseMenu(game);
  }

  x = startX + diff * i;
  if (guiState.doButton({Lerp(x + buttonWidth, x, BounceOut(game->frameTimer)), 10 + (i++ * (buttonHeight + margin)), buttonWidth, buttonHeight}, "Hack Tools", 10, GUI_FLAG_CENTER_Y)) {
    ClosePauseMenu(game);
  }

  x = startX + diff * i;
  if (guiState.doButton({Lerp(x + buttonWidth, x, BounceOut(game->frameTimer)), 10 + (i++ * (buttonHeight + margin)), buttonWidth, buttonHeight}, "Exit", 10, GUI_FLAG_CENTER_Y)) {
    ClosePauseMenu(game);
    GotoScene(game, new MenuScene(game->reg));
  }

  x = startX + diff * i;
  if (guiState.doButton({Lerp(x + buttonWidth, x, BounceOut(game->frameTimer)), 10 + (i++ * (buttonHeight + margin)), buttonWidth, buttonHeight}, "Exit To Desktop", 10, GUI_FLAG_CENTER_Y)) {
    //TODO(Dustin): Prompt asking if they are sure, then clean up nicely
    CloseWindow();
    exit(EXIT_SUCCESS);
  }
}

void RenderGui(const uptr<Game> &game) {
  auto &state = game->guiState;

  if (state.banner.timeLeft >= 0.0f && state.banner.text != "") {
    DrawBanner(state.banner);
  }

  const auto tex = Assets::I()->textures[Textures::TEX_GUI];
  DrawInventory(game, state);

  const auto [cx, cy] = MousePositionCanvasSpace();
  DrawCircle(cx, cy, 2, BLUE);

  DrawHud(game, state, tex);

  if (game->state == AppState::InDialog && game->dialogTree.has_value()) {
    auto dialog = game->dialogTree.value();
    DrawDialog(dialog);
  }

  if (game->state == AppState::PauseMenu)
    DrawPauseMenu(game, state);
}

void DoAreaBanner(const uptr<Game> &game, const std::string &text) {
  if (game->guiState.banner.text != "" || game->guiState.banner.timeLeft > 0.0f) return;
  game->guiState.banner.color.a = 0.0f;
  game->guiState.banner.text = text;
  game->guiState.banner.timeLeft = BANNER_MAX_TIME;
}
