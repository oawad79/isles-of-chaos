#include "gui.hpp"

constexpr auto cellSize {16};
constexpr auto margin{1}; 

constexpr auto dialogWidth = CANVAS_WIDTH / 2;
constexpr auto dialogHeight = CANVAS_HEIGHT / 4; 

void UpdateHud(const uptr<Game>& game, GuiState& state) {
    game->reg.view<Player, Health>().each([&](auto& p, auto& health){
        const auto d = Input::I()->GetMovementVector();
        state.frameTarget = d.x * 2.0f;
        state.frameScaler = lerp(state.frameScaler, state.frameTarget, GetFrameTime() * state.speed);

        const float sign = state.frameScaler < 0 ? -1 : 1;
        const float f = sign < 0 ? -state.frameScaler : state.frameScaler;
        if (f < 0.1) state.frameScaler = 0.0f;
        if (f > 1.8 && sign > 1) state.frameScaler = 2.0f;
    });
}

void DrawHud(const uptr<Game>& game, GuiState& state, const Texture& tex){
    game->reg.view<Player, Health>().each([&](auto& p, auto& health){
        //int frame = floor(state.frameScaler);
        //if (state.frameScaler > 1.8) frame = 2;
        //const auto [rx, ry, rw, rh] = state.healthRegion;

        const float dist = health.max - health.amount;
        DrawRectangle(0, dist, 16, health.max - dist, RED);

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

InvSpacial GetInvSpacial(Inventory& inv) {
    const float width = inv.maxColumns * (cellSize + margin);
    const float height = inv.maxRows * (cellSize + margin);
    constexpr auto cwh = GUI_CANVAS_WIDTH/2;
    constexpr auto chh = GUI_CANVAS_HEIGHT*(0.75);

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

void DrawInventory(const uptr<Game>& game, GuiState& state) {
    auto drawItem = [](float x, float y, float size, Texture2D& tex, Item& item) {
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
            WHITE
        );
    };

    auto drawEquipmentSquare = [&drawItem](float x, float y, std::optional<Item> o) {
        DrawRectangle(x, y, cellSize, cellSize, {0, 0, 0, 100});
        DrawRectangleLines(x, y, cellSize, cellSize, BLACK);
        if (o) {
            auto tex = Assets::I()->textures[Textures::TEX_EQUIPMENT];
            auto item = o.value();
            drawItem(x, y, cellSize, tex, item);
        }
    };

    game->reg
        .view<Player, Inventory, Character>()
        .each([&state, &drawEquipmentSquare, &drawItem, &game]
              (auto& player, Inventory& inventory, Character& character){
        if (state.playerInvOpen) {
            const auto equiped = character.equiped;

            const auto sp = GetInvSpacial(inventory);
            const auto [scx, scy] = sp.startPos;
            auto [ex, ey] = sp.equipStartPos;
            const auto [width, height] = sp.totalSize;
            const auto [wx, wy] = sp.weaponPos;

            const auto consu = Assets::I()->textures[Textures::TEX_ITEMS];
            const auto equip = Assets::I()->textures[Textures::TEX_EQUIPMENT];

            // Draw equipment
            drawEquipmentSquare(wx, wy, equiped.weapon);

            int i = 0;
            drawEquipmentSquare(ex, ey + (cellSize + margin) * i++, equiped.helmet);
            drawEquipmentSquare(ex, ey + (cellSize + margin) * i++, equiped.chestPiece);

            auto [cx, cy] = sp.startPos;

            for (int y = 0; y < inventory.maxRows; y++) {
                for (int x = 0; x < inventory.maxColumns; x++) {
                    DrawRectangle(cx, cy, cellSize, cellSize, {0, 0, 0, 100});
                    DrawRectangleLines(cx, cy, cellSize, cellSize, BLACK);
                    auto o = inventory.getItem(x, y);
                    if (o) {
                        auto tex = (
                            o->catagory == ItemCatagory::Consumable || o->catagory == ItemCatagory::Money
                        ) ? consu
                          : equip;
                        auto item = o.value();
                        drawItem(cx, cy, cellSize, tex, item);
                    }

                    cx += cellSize + margin;
                }
                cx = scx;
                cy += cellSize + margin;
            }

            cx = sp.startPos.x;
            cy = sp.startPos.y;

            for (int y = 0; y < inventory.maxRows; y++) {
                for (int x = 0; x < inventory.maxColumns; x++) { 
                    auto o = inventory.getItem(x, y);
                    if (o) {
                        const char* text = FormatText("%d", o.value().amount);
                        const auto size = MeasureText(text, 8.0f);
                        DrawRectangle(cx + cellSize - 8, cy + cellSize - 5, 8, 8, WHITE);
                        DrawTextEx(
                            GetFontDefault(),
                            text,
                            {cx + cellSize - 6, cy + cellSize - 5},
                            8.0f,
                            8.0f,
                            BLACK
                        );

                        if (CheckCollisionPointRec(MouseCanvasPosition(game), {cx, cy, cellSize, cellSize})) {
                            // Show tooltip
                            auto item = o.value(); 

                            DrawRectangle(
                                cx - 128, cy,
                                120, 96,
                                BLACK
                            );

                            const char* name = TextFormat("Name: %s", item.name.c_str());
                            const char* catagory = TextFormat("Type: %s", ItemCatagoryS[(int)item.catagory].c_str());

                            auto yy = cy + 4;
                            const auto xx = cx - 128;

                            DrawTextEx(GetFontDefault(), name, {xx + 4, yy}, fntSize, spacing, WHITE);
                            yy += MeasureTextEx(GetFontDefault(), name, fntSize, spacing).y;
                            DrawTextEx(GetFontDefault(), catagory, {xx + 4, yy}, fntSize, spacing, WHITE);
                            yy += MeasureTextEx(GetFontDefault(), name, fntSize, spacing).y;

                            switch(item.catagory) {
                                case ItemCatagory::Consumable: {
                                    const char* effect = TextFormat("Effect: %s", ConsumableEffectS[(int)item.consumableEffect].c_str());
                                    DrawTextEx(GetFontDefault(), effect, {xx + 4, yy}, fntSize, spacing, WHITE);
                                    yy += MeasureTextEx(GetFontDefault(), effect, fntSize, spacing).y; 
                                    break;
                                }
                                case ItemCatagory::Weapon: {
                                    const char* damage = TextFormat("Damage: %f", item.effectValue);
                                    DrawTextEx(GetFontDefault(), damage, {xx + 4, yy}, fntSize, spacing, WHITE);
                                    yy += MeasureTextEx(GetFontDefault(), damage, fntSize, spacing).y; 
                                    break;
                                }
                                case ItemCatagory::Armor: {
                                    const char* armor = TextFormat("Armor: %f", item.effectValue);
                                    DrawTextEx(GetFontDefault(), armor, {xx + 4, yy}, fntSize, spacing, WHITE);
                                    yy += MeasureTextEx(GetFontDefault(), armor, fntSize, spacing).y; 
                                    break;
                                }
                            }

                            const char* value = TextFormat("Value: %d$", item.value);
                            DrawTextEx(GetFontDefault(), value, {xx + 4, yy}, fntSize, spacing, WHITE);
                            yy += MeasureTextEx(GetFontDefault(), value, fntSize, spacing).y; 

                        }
                    }
                    cx += cellSize + margin;
                }
                cx = scx;
                cy += cellSize + margin;
            }
        }
    });
}

void UpdateDialog(DialogTree& dialog, const uptr<Game>& game){
    auto& branch = dialog.branches[dialog.currentId];
    if (IsKeyPressed(KEY_A)) branch.cursor -= 1;
    if (IsKeyPressed(KEY_D)) branch.cursor += 1;
    branch.cursor %= (dialog.branches.size() + 1);

    if (IsKeyPressed(KEY_X)) {
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

void UpdateGui(const uptr<Game>& game) {
    auto& state = game->guiState;

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
        auto& dialog = game->dialogTree.value();
        UpdateDialog(dialog, game);
    }

    game->reg
        .view<Player, Body, Inventory, Character>()
        .each([&state, &game](auto& player, auto& body, Inventory& inventory, Character& character){
        if (Input::I()->OpenInv()) {
            state.playerInvOpen = !state.playerInvOpen;
        }

        if (state.playerInvOpen) {
            const auto sp = GetInvSpacial(inventory);
            const auto [scx, scy] = sp.startPos;
            auto [cx, cy] = sp.startPos;
            auto [ex, ey] = sp.equipStartPos;
            const auto [width, height] = sp.totalSize;
            const auto [wx, wy] = sp.weaponPos;

            for (int y = 0; y < inventory.maxRows; y++) {
                for (int x = 0; x < inventory.maxColumns; x++) {

                    if (IsKeyDown(KEY_LEFT_SHIFT) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (CheckCollisionPointRec(MouseCanvasPosition(game),
                                                   {cx, cy, cellSize, cellSize})) {
                            // Equip the item
                            if (auto o = inventory.getItem(x, y)) {
                                const Item item = o.value();

                                if (item.catagory == ItemCatagory::Weapon) {
                                    const auto oitem = character.equiped.weapon;
                                    character.equiped.weapon = item;
                                    inventory.putItemAt(oitem, x, y);
                                }
                            }
                        }

                        if (CheckCollisionPointRec(MouseCanvasPosition(game),
                                                   {wx, wy, cellSize, cellSize})) {
                            if (character.equiped.weapon) {
                                inventory.putItem(character.equiped.weapon.value());
                                character.equiped.weapon = std::nullopt;
                            }
                        }
                    } 

                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        if (CheckCollisionPointRec(MouseCanvasPosition(game),
                                                   {cx, cy, cellSize, cellSize})) {
                            // Drop item
                            if (auto o = inventory.getItem(x, y)) {
                                const Item item = o.value();

                                SpawnItemWithId(
                                    game->reg,
                                    {body.center().x, body.center().y},
                                    item.id
                                );

                                inventory.decOrClear(x, y);
                            }
                        }
                    }

                    cx += cellSize + margin;
                }
                cx = scx;
                cy += cellSize + margin;
            }

        }
    });
}

void DrawChoices(const std::vector<Choice>& choices, const int cursor) {
    const auto x = CANVAS_WIDTH / 2 - dialogWidth / 2;
    const auto y = CANVAS_HEIGHT - (dialogHeight + 32);
    const auto num = choices.size();
    const auto width = (dialogWidth - (margin*num)) / num;
    const auto font = GetFontDefault();

    int i = 0;
    for (const auto& [message, nextId] : choices) {
        const auto hot = i == cursor;
        const auto fillColor = hot ? BLACK : RAYWHITE;
        const auto rect = Rectangle{margin+x+(float)i*(width+margin), y+dialogHeight-20,(float)width,20};

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
        i+=1;
    }
}

void DrawBranch(const DialogBranch& branch) {
    const auto x = CANVAS_WIDTH / 2 - dialogWidth / 2;
    const auto y = CANVAS_HEIGHT - (dialogHeight + 32);
    const auto font = GetFontDefault();
    DrawTextRec(
        font,
        branch.message.c_str(),
        {x+margin,y+margin,dialogWidth-margin*2,dialogHeight-margin*2},
        10, 1.0f, true, RAYWHITE);
    if (branch.choices.size() > 0)
        DrawChoices(branch.choices, branch.cursor);
}

void DrawDialog(DialogTree& dialog) {
    // Draw border
    DrawRectangle(
        CANVAS_WIDTH/2 - dialogWidth / 2,
        CANVAS_HEIGHT-(dialogHeight+32),
        dialogWidth,
        dialogHeight,
        {0, 0, 0, 200}
    );

    const auto branch = dialog.branches[dialog.currentId];
    DrawBranch(branch);
}


void DrawBanner(BannerState& state) {
    constexpr auto fontSize = 100.0f;

    const auto& tex = Assets::I()->textures[TEX_GUI];
    const auto font = GetFontDefault();
    const auto [tw, th] = MeasureTextEx(font, state.text.c_str(), fontSize, 1.0f);

    const auto x = GetScreenWidth() / 2 - tw / 2;
    const auto y = GetScreenHeight() / 2 - th / 2 - GetScreenHeight() / 4.0f;

    const auto tileSize = 8*6.0f;

    DrawTexturePro(tex, {0, 45, 8, 38}, {x - tileSize, y, tileSize, th}, {0, 0}, 0.0f, state.color);
    for (int i = 0; i < (int)(tw / tileSize); i++) { 
      DrawTexturePro(tex, {8, 45, 16, 38}, {x + i * tileSize, y, tileSize, th}, {0, 0}, 0.0f, state.color);
    } 
    DrawTexturePro(tex, {0, 45, -8, 38}, {x + tw - 4, y, tileSize + 4, th}, {0, 0}, 0.0f, state.color);

    const Color textColor { 0, 0, 0, state.color.a };
    DrawTextRec(
        font,
        state.text.c_str(),
        {x,y,tw * 2,th},
        fontSize, 1.0f, true, textColor);
}

void RenderGui(const uptr<Game>& game) {
    auto& state = game->guiState; 

    if (state.banner.timeLeft >= 0.0f && state.banner.text != "") {
        DrawBanner(state.banner);
    }

    const auto tex = Assets::I()->textures[Textures::TEX_GUI];
    DrawInventory(game, state);

    const auto [cx, cy] = MouseCanvasPosition(game);
    DrawCircle(cx, cy, 2, BLUE);

    DrawHud(game, state, tex);

    if (game->state == AppState::InDialog && game->dialogTree.has_value()) {
        auto dialog = game->dialogTree.value();
        DrawDialog(dialog);
    }
}

void DoAreaBanner(const uptr<Game>& game, const std::string& text) {
  if (game->guiState.banner.text != "" || game->guiState.banner.timeLeft > 0.0f) return;
  game->guiState.banner.color.a = 0.0f;
  game->guiState.banner.text = text;
  game->guiState.banner.timeLeft = BANNER_MAX_TIME; 
}
