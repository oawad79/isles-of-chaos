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

Vector2 MouseCanvasPosition(const uptr<Game>& game) {
    const float mx = GetMouseX();
    const float my = GetMouseY();

    const float aspect = (float)CANVAS_HEIGHT / (float)CANVAS_WIDTH;
    const float width = GetScreenWidth();
    const float height = GetScreenHeight();

    const float ws = (float)CANVAS_WIDTH / (float)width;
    const float hs = (float)CANVAS_HEIGHT / (float)height;

    return {mx*ws, my*hs};
}
