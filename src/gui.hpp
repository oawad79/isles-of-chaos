#ifndef SKYVAULT_GUI_HPP
#define SKYVAULT_GUI_HPP

#include "game.hpp"
#include "input.hpp"
#include "player.hpp"
#include "inventory.hpp"
#include "character.hpp"

struct GuiState {
    bool playerInvOpen{false};
};

void UpdateGui(const uptr<Game>& game, GuiState& state);
void DrawGui(const uptr<Game>& game, GuiState& state);

#endif // SKYVAULT_GUI_HPP
