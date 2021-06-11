#include "game.hpp"

void LoadGame(uptr<Game>& game) {
    game->mainCanvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
    game->guiCanvas = LoadRenderTexture(GUI_CANVAS_WIDTH, GUI_CANVAS_HEIGHT);

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

    std::cout << game->physicsPaused << std::endl;
}

void RenderGame(const uptr<Game>& game) {
    for (auto* scene : game->scenes)
        scene->render(game);
}

void DoDialog(const uptr<Game>& game, const DialogTree& tree) {
    if (tree.branches.find("start") == tree.branches.end()) {
        std::cout << "Error: dialog doesn't have a 'start' branch" << std::endl;
    }

    game->dialogTree = std::optional<DialogTree>(tree);
    game->state = AppState::InDialog;
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

Vector2 MouseGuiCanvasPosition(const uptr<Game>& game) {
    const float mx = GetMouseX();
    const float my = GetMouseY();

    const float aspect = (float)GUI_CANVAS_HEIGHT / (float)GUI_CANVAS_WIDTH;
    const float width = GetScreenWidth();
    const float height = GetScreenHeight();

    const float ws = (float)GUI_CANVAS_WIDTH / (float)width;
    const float hs = (float)GUI_CANVAS_HEIGHT / (float)height;

    return {mx*ws, my*hs};
}