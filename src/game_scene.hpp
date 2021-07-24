#pragma once
#ifndef SKYVAULT_GAME_SCENE_HPP
#define SKYVAULT_GAME_SCENE_HPP

#include <entt.hpp>
#include <raylib.h>

#include "consts.hpp"
#include "scene.hpp"
#include "gui.hpp"
#include "game.hpp"
#include "level.hpp"
#include "player.hpp"
#include "body.hpp"
#include "interaction.hpp"
#include "spawners.hpp"

class GameScene : public SceneLayer {
public:
    GameScene(entt::registry& reg, const std::string gameLevel=START_GAME_LEVEL, const Vector2 tilemapPosition=Vector2{0,0});
    void load(const uptr<Game>& game) override;
    void update(const uptr<Game>& game) override;
    void render(const uptr<Game>& game) override;
    void destroy(const uptr<Game>& game) override;

    void loadLevel(const uptr<Game>& game, const std::string& which);
    void deleteLevel(const uptr<Game>& game);

private:
    enum FadeState { In, Hold, Out };
    entt::entity player{0};

    const std::string startGameLevel{""};
    const Vector2 startGamePosition{0.0f,0.0f};

    void handlePorts(const uptr<Game>& game);
    void handleDoors(const uptr<Game>& game);

    void archiveEntities(const uptr<Game>& game, int id=0);
    void unarchiveEntities(const uptr<Game>& game, int id=0);

    bool archived{false};

    float loadTimer {0.0f};

    bool enteringPort{false};
    float fadeTimer = 0.0f;
    std::string nextTilemap;
    Vector2 nextOffset{0, 0};
    Vector2 portCenter{0, 0};

    FadeState fadeState{FadeState::In};
    float blackTimer {0.0f};
};

#endif // SKYVAULT_GAME_SCENE_HPP
