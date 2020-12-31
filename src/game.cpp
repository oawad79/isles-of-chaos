#include "game.hpp"

void LoadGame(uptr<Game>& game) {
    game->mainCanvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
}
