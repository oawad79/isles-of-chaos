#include "game.hpp"

void LoadGame(uptr<Game>& game) {
    game->mainCanvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);

    game->mainCamera.rotation = 0;
    game->mainCamera.zoom = 1;
}

void PushScene(uptr<Game>& game, SceneLayer* scene) {
    scene->load(game);
    game->scenes.emplace_back(scene);
}

void PopScene(uptr<Game>& game) {
    if (game->scenes.size() > 0) {
        game->scenes[game->scenes.size() - 1]->destroy(game);
        game->scenes.pop_back();
    }
}

void GotoScene(uptr<Game>& game, SceneLayer* scene) {
    PopScene(game);
    PushScene(game, scene);
}

void UpdateGame(uptr<Game>& game) {
    for (auto* scene : game->scenes)
        scene->update(game);
}

void RenderGame(const uptr<Game>& game) {
    for (auto* scene : game->scenes)
        scene->render(game);
}
