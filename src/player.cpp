#include "player.hpp"
#include <iostream>

entt::entity SpawnPlayerHit(uptr<Game>& game, Item& item, float x, float y, Facing facing) {
    auto self = game->reg.create();
    game->reg.emplace<Item>(self, item);
    game->reg.emplace<Body>(self, Body{
        x,
        y,
        item.region.width,
        item.region.height
    });
    game->reg.emplace<PlayerHit>(self);
    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.region = item.region;
    spr.scale.x = -1;
    spr.texture = Assets::I()->textures[Textures::TEX_EQUIPMENT];
    game->reg.emplace<TimedDestroy>(self, TimedDestroy{0.2f});
    return self;
}

void UpdateCameraTracking(Body& body, Physics& physics, Camera2D& camera) {
    const auto dt = GetFrameTime();

    constexpr auto xsmooth{20.0f};
    constexpr auto ysmooth{2.0f};

    const auto ideal_x = body.x + body.width / 2 + physics.velocity.x * dt * xsmooth;
    const auto ideal_y = body.y + dt * ysmooth;

    camera.zoom = CAMERA_ZOOM;

    camera.target.x = lerp(camera.target.x, ideal_x, xsmooth * dt);
    camera.target.y = lerp(camera.target.y, ideal_y, ysmooth * dt);

    camera.offset.x = CANVAS_WIDTH/2;
    camera.offset.y = CANVAS_HEIGHT/2;

    const auto delta = GetMouseWheelMove();
}

void UpdatePlayerNormalState(
    uptr<Game>& game,
    Player &player, 
    Health &health,
    Physics& physics, 
    Body& body, 
    Character& character,
    AdvancedAnimation& sprite
) {
    const auto dt = GetFrameTime();
    const auto equiped = character.equiped;

    const auto [ax, ay] = Input::I()->GetMovementVector();

    sprite.playback = Playback::PAUSED;
    if (ax < 0) { physics.facingX = LEFT; sprite.playback = Playback::FORWARD; }
    if (ax > 0) { physics.facingX = RIGHT; sprite.playback = Playback::FORWARD; }

    if (sprite.playback == Playback::PAUSED)
        sprite.currentFrame = 0;

    sprite.scale.x = physics.facingX;

    if (!player.hit) physics.velocity.x += ax * 400.0f * dt; 
    if (player.hit) sprite.currentFrame = 2;

    if (Input::I()->Jump() && physics.on_ground) {
        physics.velocity.y -= 18000.0f * dt;
        physics.on_ground = false;
    }

    if (Input::I()->Ascend() && physics.on_ladder) {
        physics.velocity.y -= 1000.0f * dt;
    }

    if (Input::I()->Descend() && physics.on_ladder) {
        physics.velocity.y += 1000.0f * dt;
    }

    if (Input::I()->Descend() && physics.on_ladder && physics.on_ground) {
        body.y += 100.0f * dt;
    }

    if (equiped.ability.has_value()) {
        if (equiped.ability.value().id == "flippy-feather") {
            if (Input::I()->DodgeRoll() && player.dodgeRollCooloff <= 0.0f) {
                sprite.currentFrame = 0;
                sprite.currentAnimation = "rolling";
                sprite.playback = Playback::FORWARD;
                player.state = PlayerState::ROLLING;
                player.dodgeRollVel = DODGEROLL_X_SPEED * physics.facingX;
                physics.velocity.y = -10000.0f * dt;
                physics.on_ground = false;
            }
        }
    }

    player.dodgeRollCooloff -= dt;

    UpdateCameraTracking(body, physics, game->mainCamera);

    if (player.hit && game->reg.valid(player.hit.value())) {
        auto& hitBody = game->reg.get<Body>(player.hit.value());
        auto& spr = game->reg.get<Sprite>(player.hit.value());

        spr.scale.x = physics.facingX;

        hitBody.x = body.center().x - hitBody.width / 2 + hitBody.width * physics.facingX;
        hitBody.y = body.center().y - hitBody.height / 2;
    } else {
        player.hit = std::nullopt;
    }

    // Attack
    if (Input::I()->Attack() && !player.hit) {
        if (auto o = equiped.weapon) {
            auto weaponItem = o.value();

            sprite.currentFrame = 2;
            // physics.velocity.x = 0.0f;

            player.hit = std::optional{
                SpawnPlayerHit(
                    game,
                    weaponItem,
                    body.center().x - weaponItem.region.width / 2 + weaponItem.region.width * physics.facingX,
                    body.center().y - weaponItem.region.height / 2,
                    physics.facingX)
            };
        }
    }

    // Handle enemy collisions
    auto view = game->reg.view<Actor, Body, Physics>(entt::exclude<Player, Disabled>);
    for (auto& ent : view) {
      auto& eactor = game->reg.get<Actor>(ent);
      if (!IsEnemy(eactor)) continue;
  
      auto& ebody = game->reg.get<Body>(ent);
     
      if (CheckCollisionRecs(body, ebody)) {
        health.hit(eactor.enemyStats.damage);
      }

    }
}

void UpdatePlayerRollingState(
    uptr<Game>& game,
    Player &player, 
    Physics& physics, 
    Body& body, 
    Character& character, 
    AdvancedAnimation& sprite
) { 
  if (sprite.currentFrame >= sprite.animations[sprite.currentAnimation].frames.size() - 1) {
    sprite.currentFrame = 0;
    sprite.currentAnimation = "moving";
    player.state = PlayerState::NORMAL; 
    player.dodgeRollCooloff = DODGEROLL_COOLOFF;
  }

  physics.velocity.x = player.dodgeRollVel * GetFrameTime();

  UpdateCameraTracking(body, physics, game->mainCamera);
}

void UpdatePlayer(uptr<Game>& game, entt::registry& reg) {
    auto view = reg.view<
      Player, 
      Health, 
      Physics, 
      Body, 
      Character, 
      AdvancedAnimation
    >();

    view.each([&game](auto &player, auto& health, auto& physics, auto& body, auto& character, AdvancedAnimation& sprite){
        if (player.state == PlayerState::NORMAL) {
          UpdatePlayerNormalState(game, player, health, physics, body, character, sprite);
        } else if (player.state == PlayerState::ROLLING) {
          UpdatePlayerRollingState(game, player, physics, body, character, sprite);
        }
    });
}
