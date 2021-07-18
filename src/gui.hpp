#ifndef SKYVAULT_GUI_HPP
#define SKYVAULT_GUI_HPP

#include <raylib.h>

#include "game.hpp"
#include "gui_state.hpp"
#include "menu_scene.hpp"
#include "input.hpp"
#include "player.hpp"
#include "inventory.hpp"
#include "character.hpp"
#include "spawners.hpp"

void UpdateGui(const uptr<Game>& game);
void RenderGui(const uptr<Game>& game);

void ShowPauseMenu(const uptr<Game>& game);
void ClosePauseMenu(const uptr<Game>& game);

void DoAreaBanner(const uptr<Game>& game, const std::string text);

#endif // SKYVAULT_GUI_HPP
