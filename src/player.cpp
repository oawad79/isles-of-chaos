#include "player.hpp"
#include <iostream>

entt::entity SpawnPlayerHit(uptr<Game>& game, Item& item, float x, float y, Facing facing) {
    auto self = game->reg.create();

    game->reg.emplace<PlayerHit>(self);
    auto& spr = game->reg.emplace<Sprite>(self);
    spr.T = Type::SPRITE;
    spr.tint = WHITE;
    spr.region = item.region;
    spr.scale.x = facing == Facing::LEFT ? -1 : 1;

    auto width = item.region.width;
    auto height = item.region.height;

    spr.texture = Assets::I()->textures[Textures::TEX_EQUIPMENT];

    //if (facing == Facing::UP) {
    //  spr.rotation = 90.0f;
    //  std::swap(width, height);
    //}

    game->reg.emplace<Item>(self, item);

    game->reg.emplace<Body>(self, Body{
        x,
        y,
        width,
        height
    });

    game->reg.emplace<TimedDestroy>(self, TimedDestroy{0.2f});
    return self;
}

void UpdateCameraTracking(Body& body, Physics& physics, Camera2D& camera) {
    const auto dt = GetFrameTime();

    camera.target = Vector2 {
        body.center().x,
        body.center().y,
    };

    camera.offset.x = floor(CANVAS_WIDTH/2.0f);
    camera.offset.y = floor(CANVAS_HEIGHT/2.0f);

    const auto scroll = GetMouseWheelMove();
    if (scroll != 0) {
        camera.zoom += scroll * 0.02f;
    }

    if (IsKeyPressed(KEY_EQUAL))
      camera.zoom = 0;
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
    const bool inCutscene = game->stage.currentPlay != std::nullopt;

    const auto dt = GetFrameTime();
    const auto equiped = character.equiped;

    const auto [ax, ay] = Input::I()->GetMovementVector();

    sprite.playback = Playback::PAUSED;

    if (!inCutscene) {
      if (ax < 0) {
        physics.facingX = LEFT;
        sprite.playback = Playback::FORWARD;
        player.facing = -1.0f;
      }
      if (ax > 0) {
        physics.facingX = RIGHT;
        sprite.playback = Playback::FORWARD;
        player.facing = 1.0f;
      }
    }

    if (sprite.playback == Playback::PAUSED)
        sprite.currentFrame = 0;

//    sprite.currentFrame = 1;

    sprite.scale.x = player.facing;

    if (!inCutscene) {
        if (!player.hit) physics.velocity.x += ax * 400.0f * dt; 
        if (player.hit && !player.isJumping) sprite.currentFrame = 2;

        if (Input::I()->Jump() && physics.on_ground) {
            physics.velocity.y -= 330.0f;
            physics.on_ground = false;
            player.isJumping = true;
        }

        // Handle jump sprites
        if (player.isJumping) {

          if (physics.velocity.y < 0) {
            sprite.currentAnimation = "jumping";
            sprite.currentFrame = 0;
            sprite.playback = Playback::PAUSED;
          } else { 
            sprite.currentFrame = 1;
          } 

          if (physics.on_ground) {
            player.isJumping = false;
            sprite.currentAnimation = "moving";
            sprite.playback = Playback::FORWARD;
          }
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

        if (character.hasAbility()) {
            if (character.getAbility().value().id == "flippy-feather") {
                if (Input::I()->DodgeRoll() && player.dodgeRollCooloff <= 0.0f) {
                    sprite.currentFrame = 0;
                    sprite.currentAnimation = "rolling";
                    sprite.playback = Playback::FORWARD;
                    player.state = PlayerState::ROLLING;
                    player.dodgeRollVel = DODGEROLL_X_SPEED * player.facing;
                    physics.velocity.y = -160.0f;
                    physics.velocity.x *= 1.5f;
                    physics.on_ground = false;
                }
            }
        }

        player.dodgeRollCooloff -= dt;

        constexpr auto startPerc = 0.45f;

        if (player.hit && game->reg.valid(player.hit.value())) { 
            auto& hitBody = game->reg.get<Body>(player.hit.value());
            auto& spr = game->reg.get<Sprite>(player.hit.value());

            spr.scale.x = sprite.scale.x;

            hitBody.x = body.center().x - hitBody.width / 2 + ((hitBody.width * 0.9f) * player.swingAnimTimer) * player.facing;
            hitBody.y = body.center().y - hitBody.height / 2 - 1;

            player.swingAnimTimer = Lerp(player.swingAnimTimer, 1.0f, GetFrameTime()*SWING_ANIM_SPEED);
        } else {
            player.hit = std::nullopt;
        }

        // Attack
        if (Input::I()->Attack() && !player.hit && player.attackCooloff <= -0.1f) {
            if (auto o = character.getWeapon()) {
                auto weaponItem = o.value();

                player.attackCooloff = weaponItem.usageCooloff;
                if (!player.isJumping)
					sprite.currentFrame = 2;
                player.swingAnimTimer = startPerc;

                player.hit = std::optional{
                    SpawnPlayerHit(
                        game,
                        weaponItem,
                        body.center().x - weaponItem.region.width / 2 + ((weaponItem.region.width * 0.9f) * startPerc) * player.facing,
                        body.center().y - weaponItem.region.height / 2 - 1,
                        Facing::LEFT)
                };
            }
        }

        if (player.attackCooloff <= 0.0f && player.attackCooloff > -0.1f && !player.isJumping) {
            sprite.currentFrame = 3;
        }

        if (player.attackCooloff > -0.1f) {
            player.attackCooloff -= GetFrameTime();
            std::cout << player.attackCooloff << std::endl;
        }

        // Handle enemy collisions
        auto view = game->reg.view<Actor, Body, Physics>(entt::exclude<Player, Disabled>);
        for (auto& ent : view) {
            auto& eactor = game->reg.get<Actor>(ent);
            if (!IsEnemy(eactor)) continue;
        
            auto& ebody = game->reg.get<Body>(ent);
            
            if (CheckCollisionRecs(body, ebody) && health.canHit()) {
                health.hit(eactor.enemyStats.damage);

                // Do knockback
                float side = body.center().x > ebody.center().x ? 1.f : -1.f;
                physics.velocity.x = 50 * side;
                physics.velocity.y = -130;

                if (game->reg.has<Physics>(ent)) {
                  auto &ephysics = game->reg.get<Physics>(ent);
                  ephysics.velocity.y = physics.velocity.y;
                  ephysics.velocity.x = -physics.velocity.x;
                }
            }
        }
    }

    UpdateCameraTracking(body, physics, game->mainCamera);

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
