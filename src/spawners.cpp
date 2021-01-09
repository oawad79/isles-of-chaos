#include "spawners.hpp"

entt::entity SpawnNone(const uptr<Game>& game, const Vector2 position) {
    return entt::entity{0};
}

entt::entity SpawnPlayer(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 10;
    body.height = 20;

    auto& spr = game->reg.emplace<SimpleAnimation>(self);
    spr.T = Type::ANIMATION;
    spr.tint = WHITE;
    spr.region = {0, 0, 12, 20};
    spr.number_of_frames = 3;
    spr.offset.x = 2;
    spr.texture = Assets::I()->textures[Textures::TEX_PLAYER];

    auto& player = game->reg.emplace<Player>(self);
    auto& physics = game->reg.emplace<Physics>(self);

    game->reg.emplace<Character>(self);
    game->reg.emplace<Inventory>(self, Inventory((size_t)6, (size_t)4));

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

entt::entity SpawnItem(const uptr<Game>& game, const Vector2 position) {
    return entt::entity{0};
}

entt::entity SpawnItemWithId(
  const uptr<Game>& game,
  const Vector2 position,
  const std::string& id) {
    auto self = game->reg.create();

    const auto itemData = Assets::I()->getItemInfo(id);
    game->reg.emplace<Item>(self, itemData);

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = itemData.region.width;
    body.height = itemData.region.height;

    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;

    if (itemData.catagory == ItemCatagory::Consumable) {
        spr.texture = Assets::I()->textures[Textures::TEX_ITEMS];
    } else if (itemData.catagory == ItemCatagory::Weapon) {
        spr.texture = Assets::I()->textures[Textures::TEX_EQUIPMENT];
    }

    spr.region = itemData.region;

    auto& physics = game->reg.emplace<Physics>(self);

    auto& inter = game->reg.emplace<Interaction>(self);
    inter.action = [&](auto e, entt::registry& r){
        auto item = r.get<Item>(e);
        r.view<Player, Inventory>().each([&](auto& p, auto& inv){
            inv.putItem(item);
        });
        r.destroy(e);
    };

    return self;
}

void SpawnEntitiesFromTileMap(const Tilemap* map, const uptr<Game>& game) {
    for (const auto& obj : map->objects) {
        if (obj.type == EntType::Item) {
            SpawnItemWithId(game, {obj.x, obj.y}, obj.id);
        } else {
            Spawn(obj.type, game, {obj.x, obj.y});
        }
    }
}

entt::entity Spawn(const EntType which, const uptr<Game>& game, const Vector2 position) {
    return SpawnerMap[(int)which](game, position);
}
