#include "gui.hpp"

constexpr auto cellSize{16};

constexpr auto dialogWidth = (float)GUI_CANVAS_WIDTH - (GUI_CANVAS_WIDTH / 4.0f);
constexpr auto dialogHeight = GUI_CANVAS_HEIGHT / 3.0f;

const auto dialogX = GUI_CANVAS_WIDTH / 2 - dialogWidth / 2;
const auto dialogY = GUI_CANVAS_HEIGHT - (dialogHeight + 16);

constexpr auto margin{0};

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

  float ex = scx + width;
  float ey = scy;

  float wx = ex - cellSize - margin;
  float wy = ey + cellSize + margin;

  return {{scx, (float)scy},
          {ex + 2, ey},
          {width, height},
          {wx, wy}};
}

constexpr auto fntSize = 10.0f;
constexpr auto spacing = 1.0f;

void DrawSlot(const Slot& slot, float cx, float cy, bool isEquip = false) {
  const float x = cx + slot.column * cellSize;
  const float y = cy + slot.row * cellSize;
  const int index = slot.column + slot.row * 5;
  const bool even = index % 2 == 0;

  Art::I()->drawRect(x, y, cellSize, cellSize, Color{0, 0, 0, (unsigned char)(even?230:250)}, 1.0f);

  if (slot.it) {
    const auto& item = *slot.it;
    const auto w = (isEquip ? 1 : item.width) * cellSize;
    const auto h = (isEquip ? 1 : item.height) * cellSize;
    const auto rw = item.region.width;
    const auto rh = item.region.height;
    const auto xx = x + w / 2 - rw / 2;
    const auto yy = y + h / 2 - rw / 2;

    const auto hot = CheckCollisionPointRec(
      GetMouseGuiPosition(),
      {(float)x, (float)y, (float)w, (float)h}
    );

    if (hot) {
      Art::I()->drawRectL(x, y, w, h, Art::I()->strobeLight(), 2.0f);
    } else {
      Art::I()->drawRectL(x, y, w, h, Color{255, 255, 255, 20}, 2.0f);
    }

    if (slot.it->catagory == ItemCatagory::Weapon)
      Art::I()->drawEquipSprite({xx, yy, rw, rh}, item.region, WHITE, 3.0f);
    else
      Art::I()->drawItemSprite({xx, yy, rw, rh}, item.region, WHITE, 3.0f);
  }
}

void DrawInventory(const uptr<Game> &game, GuiState &state) {
  const auto drawSlots = [&](auto &player, Inventory &inventory, Character &character) {
    if (state.playerInvOpen) {
      const auto spacial = GetInvSpacial(inventory);
      const auto [cx, cy] = spacial.startPos;
      const auto [ex, ey] = spacial.equipStartPos;
      for (const auto &slot : inventory.slots) DrawSlot(slot, cx, cy);
      for (const auto &slot : character.equiped.slots) DrawSlot(slot, ex, ey, true);
    }
  };

  game->reg.view<Player, Inventory, Character>().each(drawSlots);
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

  const auto updatePlayerInventory = [&state, &game](auto &player, auto &body, Inventory &inventory, Character &character){
    if (Input::I()->OpenInv()) {
      state.playerInvOpen = !state.playerInvOpen;
      game->state = state.playerInvOpen ? AppState::InDialog : AppState::Running;
    }

    if (state.playerInvOpen) {
      const auto sp = GetInvSpacial(inventory);

      for (auto &slot : character.equiped.slots) {
        const auto [cx, cy] = sp.equipStartPos;
        const float rx = cx + slot.column * cellSize;
        const float ry = cy + slot.row * cellSize;

        const auto hot = CheckCollisionPointRec(
          GetMouseGuiPosition(),
          {(float)rx, (float)ry, cellSize, cellSize}
        );

        if (!slot.it) continue;

        if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !inventory.isFull()) {
          inventory.putItem(slot.it.value());
          slot.it = std::nullopt;
        }
      }

      for (auto &slot : inventory.slots) {
        const auto [cx, cy] = sp.startPos;
        if (!slot.it) continue;

        const float rx = cx + slot.column * cellSize;
        const float ry = cy + slot.row * cellSize;

        const auto hot = CheckCollisionPointRec(
          GetMouseGuiPosition(),
          {(float)rx, (float)ry, (float)cellSize * slot.it->width, (float)cellSize * slot.it->height}
        );

        if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          slot.it = character.equip(slot.it.value());
        }
      }
    }
  };

  game->reg.view<Player, Body, Inventory, Character>().each(updatePlayerInventory);
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
