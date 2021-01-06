#include "gui.hpp"

constexpr auto cellSize {16};
constexpr auto margin{1};

void DrawInventory(const uptr<Game>& game, GuiState& state) {
    game->reg
        .view<Player, Inventory>()
        .each([&state](auto& player, Inventory& inventory){
        if (state.playerInvOpen) {
            const auto width = inventory.maxColumns * (cellSize + margin);
            const auto height = inventory.maxRows * (cellSize + margin);
            constexpr auto cwh = CANVAS_WIDTH/2;
            constexpr auto chh = CANVAS_HEIGHT*(0.75);

            const auto scx = cwh - width / 2;
            const auto scy = chh - height / 2;
            float cx = scx;
            float cy = scy;

            const auto tex = Assets::I()->textures[Textures::TEX_ITEMS];

            for (int y = 0; y < inventory.maxRows; y++) {
                for (int x = 0; x < inventory.maxColumns; x++) {
                    DrawRectangle(cx, cy, cellSize, cellSize, {0, 0, 0, 100});
                    DrawRectangleLines(cx, cy, cellSize, cellSize, BLACK);

                    auto o = inventory.getItem(x, y);
                    if (o) {
                        auto item = o.value();

                        DrawTexturePro(
                            tex,
                            item.region,
                            {cx, cy, cellSize, cellSize},
                            {0, 0},
                            0.0,
                            WHITE);
                    }

                    cx += cellSize + margin;
                }
                cx = scx;
                cy += cellSize + margin;
            }
        }
    });
}

void UpdateGui(const uptr<Game>& game, GuiState& state) {
    game->reg
        .view<Player, Inventory>()
        .each([&state, &game](auto& player, auto& inventory){
        if (Input::I()->OpenInv()) {
            state.playerInvOpen = !state.playerInvOpen;
            // game->state =
            //     state.playerInvOpen
            //     ? AppState::Paused
            //     : AppState::Running;
        }

        if (state.playerInvOpen) {

        }
    });
}

void DrawGui(const uptr<Game>& game, GuiState& state) {
    DrawInventory(game, state);
}
