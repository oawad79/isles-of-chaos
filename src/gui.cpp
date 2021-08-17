#include "gui.hpp"

constexpr auto cellSize{16};
constexpr auto margin{1};

constexpr auto dialogWidth = (float)GUI_CANVAS_WIDTH - (GUI_CANVAS_WIDTH / 4.0f);
constexpr auto dialogHeight = GUI_CANVAS_HEIGHT / 3.0f;

const auto dialogX = GUI_CANVAS_WIDTH / 2 - dialogWidth / 2;
const auto dialogY = GUI_CANVAS_HEIGHT - (dialogHeight + 16);

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

  const auto scx = cwh + width / 2 + cellSize;
  const auto scy = chh - height / 2 - height / 2;
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
      if (o.amount > 1) {
        const char *text = TextFormat("%d", o.amount);
        DrawRectangle(cx + cellSize - 8, cy + cellSize - 5, 8, 8, WHITE);
        DrawTextEx(GetFontDefault(), text, {cx + cellSize - 6, cy + cellSize - 5}, 8.0f, 8.0f, BLACK);
      }

      drawTooltip(o, cx, cy);
    }
  };

  game->reg
          .view<Player, Inventory, Character>()
          .each([&](auto &player, Inventory &inventory, Character &character) {
            if (state.playerInvOpen) {
              auto [cx, cy] = GetInvSpacial(inventory).startPos;
              for (const auto &slot : inventory.slots) drawSlot(slot, cx, cy);
              for (const auto &slot : character.equiped.slots) drawSlot(slot, cx, cy);
            }
          });
}

void UpdateDialog(DialogTree &dialog, const uptr<Game> &game) {
  auto &branch = dialog.branches[dialog.currentId];
  if (IsKeyPressed(KEY_A)) branch.cursor -= 1;
  if (IsKeyPressed(KEY_D)) branch.cursor += 1;
  branch.cursor %= dialog.branches.size() + 1;

  if (Input::I()->Interact()) {
    if (branch.choices.empty()) {
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

void UpdateBanner(BannerState& banner) {
  if (banner.state == BFS_IN) {
    banner.alpha += GetFrameTime();
    if (banner.alpha >= 1.0f) {
      banner.alpha = 1.0f;
      banner.timeLeft = 1.0f;
      banner.state = BFS_HOLD;
    }
  } else if (banner.state == BFS_HOLD) {
    if (banner.timeLeft > 0.0f) {
      banner.timeLeft -= GetFrameTime() * 0.75f;
    } else {
      banner.timeLeft = 0.0f;
      banner.state = BFS_OUT;
    }
  } else if (banner.state == BFS_OUT) {
    if (banner.alpha > 0.0f) {
      banner.alpha -= GetFrameTime();
      if (banner.alpha < 0.0f) { banner.alpha = 0.0f; }
    } else {
      banner.alpha = 0.0f;
      banner.text = "";
      banner.state = BFS_NONE;
    }
  }


//    if (banner.timeLeft <= 0.0f)
//      banner.text = "";
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
  UpdateBanner(state.banner);
  UpdateHud(game);

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

              for (auto &slot : character.equiped.slots) {
                const int rx = cx + slot.column * (cellSize + margin);
                const int ry = cy + slot.row * (cellSize + margin);

                const auto hot = CheckCollisionPointRec(GetMouseGuiPosition(), {(float)rx,
                                                                                (float)ry,
                                                                                cellSize,
                                                                                cellSize});
                if (!slot.it) continue;

                if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inventory.isFull()) {
                  inventory.putItem(slot.it.value());
                  slot.it = std::nullopt;
                }
              }

              for (auto &slot : inventory.slots) {
                const int rx = cx + slot.column * (cellSize + margin);
                const int ry = cy + slot.row * (cellSize + margin);

                const auto hot = CheckCollisionPointRec(GetMouseGuiPosition(), {(float)rx,
                                                                                (float)ry,
                                                                                cellSize,
                                                                                cellSize});
                if (!slot.it) continue;
                if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                  slot.it = character.equip(slot.it.value());
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
  if (state.state == BFS_NONE) return;
  constexpr auto fontSize = 20.0f;

  const auto &tex = Assets::I()->textures[TEX_GUI];
  const auto font = Assets::I()->fonts[FONT_FANCY];

  const auto [tw, th] = MeasureTextEx(font, state.text.c_str(), (int)fontSize, 1.0f);

  const auto x = GUI_CANVAS_WIDTH / 2 - tw / 2;
  const auto y = GUI_CANVAS_HEIGHT / 2 - th / 2 - GUI_CANVAS_HEIGHT / 4.0f;

  const auto tileSize = 8 * 6.0f;

  float s = 255.0f * state.alpha * 0.75f;
  const auto a = static_cast<uint8_t>(s);
  DrawRectangle(0, y - th / 2, GUI_CANVAS_WIDTH, th*2, {0, 0, 0, a});

  const Color textColor{255, 255, 255, static_cast<uint8_t>(255.0f * state.alpha * state.alpha * 0.9f)};
  DrawTextEx(font, state.text.c_str(), Vector2{x, y}, fontSize, 1.0f, textColor);
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

  DrawBanner(state.banner);

  const auto tex = Assets::I()->textures[Textures::TEX_GUI];
  DrawInventory(game, state);

  const auto [cx, cy] = MousePositionCanvasSpace();
  DrawCircle(cx, cy, 2, BLUE);

  DrawHud(game);

  if (game->state == AppState::InDialog && game->dialogTree.has_value()) {
    auto dialog = game->dialogTree.value();
    DrawDialog(dialog);
  }

  if (game->state == AppState::PauseMenu)
    DrawPauseMenu(game, state);
}

void DoAreaBanner(const uptr<Game> &game, const std::string text) {
  if (game->guiState.banner.text != "" || game->guiState.banner.timeLeft > 0.0f) return;
  game->guiState.banner.alpha = 0.0f;
  game->guiState.banner.state = BFS_IN;
  game->guiState.banner.text = text;
  game->guiState.banner.timeLeft = BANNER_MAX_TIME;
}
