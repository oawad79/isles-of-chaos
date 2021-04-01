#ifndef SKYVAULT_GUI_HPP
#define SKYVAULT_GUI_HPP

#include "game.hpp"
#include "input.hpp"
#include "player.hpp"
#include "inventory.hpp"
#include "character.hpp"
#include "spawners.hpp"

struct GuiState {
    bool playerInvOpen{false};

    Rectangle healthRegion{64, 16, 32, 16};
    int healthFrame{0};
    float healthFrameTimer{0.0f};
    float speed{6.0f};
    float frameScaler{0.0f}; // Goes from -2.0 to 2.0
    float frameTarget{0.0f};
};

void UpdateGui(const uptr<Game>& game, GuiState& state);
void DrawGui(const uptr<Game>& game, GuiState& state);

#endif // SKYVAULT_GUI_HPP
