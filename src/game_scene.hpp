#ifndef SKYVAULT_GAME_SCENE_HPP
#define SKYVAULT_GAME_SCENE_HPP

#include <entt.hpp>

#include "scene.hpp"
#include "game.hpp"
#include "level.hpp"
#include "player.hpp"
#include "body.hpp"
#include "interaction.hpp"
#include "spawners.hpp"

class GameScene : public SceneLayer {
public:
    GameScene(entt::registry& reg);
    void load(uptr<Game>& game) override;
    void update(uptr<Game>& game) override;
    void render(const uptr<Game>& game) override;
    void destroy(uptr<Game>& game) override;
private:
    entt::entity player{0};

    void handlePorts(uptr<Game>& game);

    void archiveEntities(uptr<Game>& game, int id=0);
    void unarchiveEntities(uptr<Game>& game, int id=0);
    bool archived{false};

    bool enteringPort{false};
    bool fadeOut{false};
    float fadeTimer = 0.0f;
    std::string nextTilemap{""};
    Vector2 nextOffset{0, 0};
    Vector2 respawnLocation{0, 0};
};

#endif // SKYVAULT_GAME_SCENE_HPP
