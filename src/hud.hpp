#ifndef SKYVAULT_HUD_HPP
#define SKYVAULT_HUD_HPP

#include <raylib.h>
#include <entt.hpp>

#include "assets.hpp"
#include "gui_state.hpp"
#include "game.hpp"
#include "player.hpp"
#include "character.hpp"

void UpdateHud(const uptr<Game> &game);
void DrawHud(const uptr<Game> &game);

#endif//SKYVAULT_HUD_HPP
