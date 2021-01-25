#include "spawners.hpp"

entt::entity BasicBodyAndSprite(entt::registry& reg, const Body& body, const Rectangle region) {
    auto self = reg.create();
    reg.emplace<Body>(self, body);
    auto& spr = reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.region = region;
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];
    return self;
}

entt::entity SpawnNone(const uptr<Game>& game, const Vector2 position) {
    return entt::entity{0};
}

entt::entity SpawnPlayer(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    std::cout << "Spawning Player\n" << std::endl;

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 10;
    body.height = 20;

    auto& spr = game->reg.emplace<SimpleAnimation>(self);
    spr.T = Type::ANIMATION;
    spr.tint = WHITE;
    spr.region = {0, 0, 12, 20};
    spr.number_of_frames = 4;
    spr.offset.x = -1;
    spr.texture = Assets::I()->textures[Textures::TEX_PLAYER];

    auto& player = game->reg.emplace<Player>(self);
    auto& physics = game->reg.emplace<Physics>(self);
    auto& health = game->reg.emplace<Health>(self);

    auto& chr = game->reg.emplace<Character>(self);
    // chr.equiped.weapon = std::optional{Assets::I()->getItemInfo("small-sword")};

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
    spr.offset.x += 4;
    spr.region = (Rectangle){0, 16, 16, 16};
    spr.number_of_frames = 4;

    auto& physics = game->reg.emplace<Physics>(self);
    game->reg.emplace<Health>(self);

    auto& actor = game->reg.emplace<Actor>(self);
    actor.type = ActorType::ZAMBIE;

    return self;
}

entt::entity SpawnDreadShark(const uptr<Game>& game, const Vector2 position) {
    const auto self = game->reg.create();
    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 32;
    body.height = 48;

    auto& spr = game->reg.emplace<SimpleAnimation>(self);
    spr.T = Type::ANIMATION;
    // spr.offset.x += 5;
    // spr.offset.y += 6;
    spr.tint = WHITE;
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];
    spr.region = (Rectangle){64, 16, 32, 48};
    spr.number_of_frames = 1;

    game->reg.emplace<Health>(self);

    auto& phy = game->reg.emplace<Physics>(self);
    phy.type = PhysicsType::KINEMATIC;

    auto& actor = game->reg.emplace<Actor>(self);
    actor.type = ActorType::DREAD_SHARK;

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

entt::entity SpawnZambie(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 12;
    body.height = 20;

    auto& spr = game->reg.emplace<SimpleAnimation>(self);
    spr.T = Type::ANIMATION;
    spr.tint = WHITE;
    spr.region = {96, 0, 12, 20};
    spr.number_of_frames = 4;
    spr.offset.x = -1;
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];

    auto& physics = game->reg.emplace<Physics>(self);
    game->reg.emplace<Health>(self);

    auto& actor = game->reg.emplace<Actor>(self);
    actor.type = ActorType::ZAMBIE;

    game->reg.emplace<Loot>(self, Assets::I()->getLootInfo("zambie"));

    return self;
}

entt::entity SpawnChest(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 24;
    body.height = 17;

    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];
    spr.region = Rectangle{96, 31, 24, 17};

    auto& inter = game->reg.emplace<Interaction>(self);
    inter.action = [&](auto e, entt::registry& r){
        const auto loot = r.get<Loot>(e);
        const auto body = r.get<Body>(e);

        auto& spr = r.get<Sprite>(e);
        spr.region.x += spr.region.width;
        spr.region.height += 1;
        spr.region.y -= 1;

        SpawnLoot(
            r,
            loot,
            body.center(),
            { -body.width + 4, body.width - 4 },
            Range2D{
                Range1D{ -10000.0f, 10000.0f },
                Range1D{ 10000.0f, 20000.0f }
            });

        r.remove<Interaction>(e);
    };

    auto& physics = game->reg.emplace<Physics>(self);

    return self;
}

entt::entity SpawnWater(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 16;
    body.height = 16;

    game->reg.emplace<Water>(self);

    return self;
}

entt::entity SpawnParticle(const uptr<Game>& game, const Vector2 position) {
    return game->reg.create();
}

entt::entity SpawnItem(const uptr<Game>& game, const Vector2 position) {
    return entt::entity{0};
}

entt::entity SpawnItemWithId(
  entt::registry& reg,
  const Vector2 position,
  const std::string& id) {
    auto self = reg.create();

    const auto itemData = Assets::I()->getItemInfo(id);
    reg.emplace<Item>(self, itemData);

    auto& body = reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = itemData.region.width;
    body.height = itemData.region.height;

    auto& spr = reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.layer = 1.0f;

    if (itemData.catagory == ItemCatagory::Consumable || itemData.catagory == ItemCatagory::Money) {
        spr.texture = Assets::I()->textures[Textures::TEX_ITEMS];
    } else if (itemData.catagory == ItemCatagory::Weapon) {
        spr.texture = Assets::I()->textures[Textures::TEX_EQUIPMENT];
    } else {
        std::cout << "WARNING::: Unsupported item category\n" << std::endl;
    }

    spr.region = itemData.region;

    auto& physics = reg.emplace<Physics>(self);

    auto& inter = reg.emplace<Interaction>(self);
    inter.action = [&](auto e, entt::registry& r){
        auto item = r.get<Item>(e);
        r.view<Player, Inventory>().each([&](auto& p, auto& inv){
            inv.putItem(item);
        });
        r.destroy(e);
    };

    return self;
}

void SpawnLoot(
    entt::registry& reg,
    const Loot& loot,
    Vector2 where,
    Range1D xRange,
    Range2D velocityRange
){
    const auto dt = GetFrameTime();
    for (const auto slot : loot.slots) {
        for (int i = 0; i < RandInt(slot.amount.min, slot.amount.max); i++) {
            auto itemEnt = SpawnItemWithId(reg, where, slot.itemId);
            auto& itemBody = reg.get<Body>(itemEnt);
            itemBody.x -= itemBody.width / 2;
            itemBody.y -= itemBody.height / 4;

            itemBody.x += RandInt(xRange.min, xRange.max);

            auto& physics = reg.get<Physics>(itemEnt);
            physics.velocity.x = RandFloat(velocityRange.xMin, velocityRange.xMax) * dt;
            physics.velocity.y = RandFloat(velocityRange.yMin, velocityRange.yMax) * dt * -1.0f;
        }
    }
}

void SpawnEntitiesFromTileMap(Tilemap* map, const uptr<Game>& game) {
    if (map->spawnedEntities) return;

    for (auto& obj : map->objects) {
        const auto x = obj.x + obj.offset.x;
        const auto y = obj.y + obj.offset.y;
        if (obj.type == EntType::Item) {
            SpawnItemWithId(game->reg, {x, y}, obj.id);
        } else if (obj.type == EntType::Water) {
            const auto ent = SpawnWater(game, {x, y});
            auto& body = game->reg.get<Body>(ent);
            body.width = obj.width;
            body.height = obj.height;
        } else if (obj.type == EntType::Chest){
            auto chest = Spawn(obj.type, game, {x, y});
            if (obj.props.find("loot") == obj.props.end())
                std::cout << "WARNING::: chest is missing loot" << std::endl;
            else
                game->reg.emplace<Loot>(chest, Assets::I()->getLootInfo(obj.props["loot"]));
        } else {
            Spawn(obj.type, game, {x, y});
        }
    }

    map->spawnedEntities = true;
}

entt::entity Spawn(const EntType which, const uptr<Game>& game, const Vector2 position) {
    return SpawnerMap[(int)which](game, position);
}
