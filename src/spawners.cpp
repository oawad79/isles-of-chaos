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

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 8;
    body.height = 18;

    auto& spr = game->reg.emplace<AdvancedAnimation>(self);
    spr.T = Type::ANIMATION;
    spr.tint = WHITE;
    spr.texture = Assets::I()->textures[Textures::TEX_PLAYER];
    spr.offset.y -= 2;
    spr.offset.x -= 3;

    spr.currentAnimation = "moving"; 
    spr.animations["moving"] = CreateUniformAnimation({ 0, 0, 12, 20 }, 4);
    spr.animations["rolling"] = CreateUniformAnimation({ 0, 28, 16, 20 }, 6, 200.0f / 3.0f); 
    spr.animations["jumping"] = CreateUniformAnimation({ 48, 0, 12, 20 }, 3, 0); 

    auto& player = game->reg.emplace<Player>(self);
    auto& physics = game->reg.emplace<Physics>(self);
    auto& health = game->reg.emplace<Health>(self);

    auto& chr = game->reg.emplace<Character>(self);
    auto& actor = game->reg.emplace<Actor>(self);
    actor.actorName = ActorName::Player;

    auto& inv = game->reg.emplace<Inventory>(self, Inventory((size_t)6, (size_t)4));

    chr.equip(Assets::I()->getItemInfo("small-sword"));
    inv.putItem(Assets::I()->getItemInfo("flippy-feather"));

    return self;
}

entt::entity SpawnKiwiBird(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 8;
    body.height = 8;

    auto& spr = game->reg.emplace<AdvancedAnimation>(self);
    spr.T = Type::ANIMATION;
    spr.tint = WHITE;
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];
    spr.currentAnimation = "flying";
    spr.animations["flying"] = CreateUniformAnimation({ 208, 0, 8, 8 }, 4, 100.0f);

    auto& physics = game->reg.emplace<Physics>(self);
    physics.gravityScale.y = 0.0f;
    physics.type = PhysicsType::KINEMATIC;

    auto& actor = game->reg.emplace<Actor>(self);
    actor.type = ActorType::KIWI_BIRD;

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
    spr.region = Rectangle{0, 16, 16, 16};
    spr.number_of_frames = 4;

    auto& physics = game->reg.emplace<Physics>(self);
    physics.gravityScale.y = 0.0f;

    game->reg.emplace<Health>(self);

    auto& actor = game->reg.emplace<Actor>(self);
    actor.type = ActorType::GHOST;

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
    spr.region = Rectangle{64, 16, 32, 48};
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

entt::entity SpawnNpc(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 16;
    body.height = 18;

    auto& spr = game->reg.emplace<SimpleAnimation>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.region = {80, 78, 16, 18};
    spr.offset.x = -1;
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];

    auto& physics = game->reg.emplace<Physics>(self);
    game->reg.emplace<Health>(self);

    auto& actor = game->reg.emplace<Actor>(self);
    actor.type = ActorType::NPC;

    return self;
}

entt::entity SpawnNpcWithId(const uptr<Game>& game, const Vector2 position, const std::string& id) {
    const auto self = SpawnNpc(game, position);
    auto& ent = game->reg.emplace<Ent>(self);
    ent.entType = EntType::Npc;

    if (id == "old-man") {
        auto convo = DialogTree{
            {{"start", {"Hello traveler! I don't believe we've meet, what is your name son?", "",
                               {{"Anon, sir. I just washed ashore and was in need of help", "less-rude"},
                                {"Yeah... not telling you anything", "rude"}}}},
                    {"rude", {"Okay wow, you are a rude boy..."}},
                    {"less-rude", {"Um im not even that old..."}}},
        };

        game->reg.get<Actor>(self).actorName = ActorName::OldMan;
        game->reg.emplace<DialogTree>(self, convo);

        auto &inter = game->reg.emplace<Interaction>(self);
        inter.action = [&](auto e, entt::registry &r) {
            const auto dialog = r.get<DialogTree>(e);
            DoDialog(game, dialog);
        };

    } else if (id == "fishing-man") {
        auto& sprite = game->reg.get<SimpleAnimation>(self);
        sprite.region = Rectangle{ 176, 10, 16, 22 };

        auto& body = game->reg.get<Body>(self);
        body.height = 17;

//        sprite.offset.y -= 5;
    } else {
        std::cout << "Unknown NPC with ID: '" << id << "'" << std::endl;
    }

    return self;
}

entt::entity SpawnShopKeeper(const uptr<Game>& game, const Vector2 position) {
  auto self = game->reg.create();

  auto& body = game->reg.emplace<Body>(self);
  body.x = position.x;
  body.y = position.y;
  body.width = 16;
  body.height = 18;

  auto& spr = game->reg.emplace<SimpleAnimation>(self);
  spr.T = Type::SPRITE;
  spr.tint = WHITE;
  spr.region = {104, 48, 24, 16};
  spr.offset.x = 0;
  spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];
  spr.number_of_frames = 1;

  return self;
}

entt::entity SpawnSmallWorm(const uptr<Game>& game, const Vector2 position) {
  auto self = game->reg.create();

  auto& body = game->reg.emplace<Body>(self);
  body.x = position.x;
  body.y = position.y;
  body.width = 6;
  body.height = 6;

  auto& spr = game->reg.emplace<SimpleAnimation>(self);
  spr.T = Type::ANIMATION;
  spr.tint = WHITE;
  spr.region = {72, 98, 6, 6};
  spr.number_of_frames = 5;
  spr.offset.x = 0;
  spr.offset.y = 1.5;
  spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];

  auto& physics = game->reg.emplace<Physics>(self);
  game->reg.emplace<Health>(self);

  auto& actor = game->reg.emplace<Actor>(self);
  actor.type = ActorType::BUG;

//  game->reg.emplace<Loot>(self, Assets::I()->getLootInfo("zambie"));

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

entt::entity SpawnSmallBoat(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto &body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 64;
    body.height = 24;

    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.region = {0, 128, 64, 24};
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];

    auto& physics = game->reg.emplace<Physics>(self);
    physics.type = PhysicsType::KINEMATIC;
    physics.gravityScale.y = 0.0f;

    return self;
}

entt::entity SpawnShroomba(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 16;
    body.height = 16;

    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.region = {32, 0, 16, 16};
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];

    auto& physics = game->reg.emplace<Physics>(self);
    game->reg.emplace<Health>(self);

    auto& actor = game->reg.emplace<Actor>(self);
    actor.type = ActorType::SHROOMBA;

    game->reg.emplace<Loot>(self, Assets::I()->getLootInfo("zambie"));

    return self;
}

entt::entity SpawnMimic(const uptr<Game>& game, const Vector2 position) {
    auto self = game->reg.create();

    auto& body = game->reg.emplace<Body>(self);
    body.x = position.x;
    body.y = position.y;
    body.width = 16;
    body.height = 16;

    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.region = {16, 32, 13, 13};
    spr.texture = Assets::I()->textures[Textures::TEX_ENTITIES];

    auto& physics = game->reg.emplace<Physics>(self);
    game->reg.emplace<Health>(self);

    auto& actor = game->reg.emplace<Actor>(self);
    actor.type = ActorType::SHROOMBA;

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
    auto& ent = reg.emplace<Ent>(self);
    ent.entType = EntType::Item;

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

    if (itemData.catagory == ItemCatagory::Consumable
    || itemData.catagory == ItemCatagory::Money
    || itemData.catagory == ItemCatagory::Ability) {
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
        } else if (obj.type == EntType::Npc) {
            SpawnNpcWithId(game, {x,y}, obj.id);
        } else if (obj.type == EntType::Chest){
            auto chest = Spawn(obj.type, game, {x, y});
            if (obj.props.find("loot") == obj.props.end())
                std::cout << "WARNING::: chest is missing loot" << std::endl;
            else
                game->reg.emplace<Loot>(chest, Assets::I()->getLootInfo(obj.props["loot"]));
        } else {
          if (obj.type == EntType::Player)  {
            bool playerExists = false;
            game->reg.view<Player>().each([&playerExists](auto _p){
              playerExists = true;
            });
            if (!playerExists) {
              Spawn(obj.type, game, {x, y});
            }
          } else {
            Spawn(obj.type, game, {x, y});
          };
        }
    }

    auto *tilemap = GetTilemap(game->level);

    float playerHeight = 64;
    game->reg.view<Player, Body, Character, Inventory>().each([&game, &playerHeight](auto &p, auto &body, Character &character, Inventory &inv) {
      game->respawnLocation = Vector2{body.x, body.y};
      playerHeight = body.height;
      // TODO(Dustin): Move to player spawn function
    });

    // Spawn checkpoints and kill zones
    for (auto &feat : tilemap->features) {
      if (feat.type == FeatureType::Checkpoint) {
        auto e = game->reg.create();
        auto &b = game->reg.emplace<Body>(e, Body{feat.x, feat.y, feat.width, feat.height});
        auto &i = game->reg.emplace<Interaction>(e);
        i.mode = InteractionMode::CALL_WHEN_ENTERED;
        i.icon = ActionIcon::NONE;
        i.action = [&game, playerHeight](auto e, entt::registry &reg) {
            const auto &b = reg.get<Body>(e);
            game->respawnLocation = Vector2{b.x + b.width / 2, b.y + b.height - playerHeight};
        };
      }

      if (feat.type == FeatureType::Kill) {
        auto e = game->reg.create();
        auto &b = game->reg.emplace<Body>(e, Body{feat.x, feat.y, feat.width, feat.height});
        auto &i = game->reg.emplace<Interaction>(e);
        i.mode = InteractionMode::CALL_WHEN_ENTERED;
        i.icon = ActionIcon::NONE;
        i.action = [&](auto e, entt::registry &reg) {
          game->reg.view<Player, Body, Health>().each([&game](auto &p, auto &body, auto& health) {
              body.x = game->respawnLocation.x;
              body.y = game->respawnLocation.y;

              health.hit(10);
              game->guiState.showHud();
          });
        };
      }

      if (feat.type == FeatureType::Banner) {
        auto e = game->reg.create();
        auto bounds = feat.bounds();
        auto &b = game->reg.emplace<Body>(e, Body{bounds.x, bounds.y, bounds.width, bounds.height});
        auto &i = game->reg.emplace<Interaction>(e);
        i.mode = InteractionMode::CALL_WHEN_ENTERED;
        i.icon = ActionIcon::NONE;
        i.action = [&](auto e, entt::registry &reg) {
          if (!feat.active) {
            DoAreaBanner(game, feat.target);
            feat.active = true;
          }
        };
      }
    }

    map->spawnedEntities = true;
}

entt::entity Spawn(const EntType which, const uptr<Game>& game, const Vector2 position) {
  auto self = SpawnerMap[(int)which](game, position);
  if (self != entt::entity(0))
    game->reg.emplace<Ent>(self, which);
  return self;
}
