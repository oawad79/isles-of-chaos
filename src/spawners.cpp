#include "spawners.hpp"

entt::entity SpawnNone(const uptr<Game>& game, const Vector2 position) {
    return entt::entity{0};
}

entt::entity SpawnPlayer(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 8;
    body.height = 12;

    auto& spr = game->reg.emplace<SimpleAnimation>(self);
    spr.T = Type::ANIMATION;
    spr.tint = WHITE;
    spr.region = {0, 0, 12, 20};
    spr.number_of_frames = 3;
    spr.texture = Assets::I()->textures[Textures::TEX_PLAYER];

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

entt::entity SpawnGhost(const uptr<Game>& game, const Vector2 position) {
    const auto self = game->reg.create();
    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 16;
    body.height = 16;

    auto& spr = game->reg.emplace<SimpleAnimation>(self);
    spr.T = Type::ANIMATION;
    spr.tint = WHITE;
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];
    spr.region = (Rectangle){0, 16, 16, 16};
    spr.number_of_frames = 4;

    auto& physics = game->reg.emplace<Physics>(self);
    return self;
}

entt::entity SpawnPot(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 8;
    body.height = 10;

    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];
    spr.region = Rectangle{0, 64, 8, 10};

    auto& physics = game->reg.emplace<Physics>(self);

    return self;
}


void SpawnEntitiesFromTileMap(const Tilemap* map, const uptr<Game>& game) {
    for (const auto& obj : map->objects) {
        Spawn(obj.type, game, {obj.x, obj.y});
    }
}

entt::entity Spawn(const EntType which, const uptr<Game>& game, const Vector2 position) {
    return SpawnerMap[(int)which](game, position);
}
