#include "spawners.hpp"

entt::entity SpawnPlayer(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 8;
    body.height = 12;

    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::RECTANGLE;
    spr.tint = LIME;

    auto& player = game->reg.emplace<Player>(self);
    auto& physics = game->reg.emplace<Physics>(self);

    return self;
}

entt::entity SpawnTest(const uptr<Game>& game, const Vector2 position) {
    const auto self = game->reg.create();
    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 8;
    body.height = 8;

    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::RECTANGLE;
    spr.tint = RED;

    auto& physics = game->reg.emplace<Physics>(self);
    auto& inter = game->reg.emplace<Interaction>(self);

    inter.action = [&](auto e, entt::registry& r){
    };

    return self;
}

entt::entity Spawn(const std::string& which, const uptr<Game>& game, const Vector2 position) {
    if (SpawnerMap.find(which) == SpawnerMap.end()) {
        std::cout << "ERROR::Spawn: Entity type: '"
                  << which
                  << "' does not exist."
                  << std::endl;
        return entt::entity(0);
    }

    return SpawnerMap[which](game, position);
}
