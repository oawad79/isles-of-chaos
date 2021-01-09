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
    const auto ideal_y = body.y + body.height / 2 + physics.velocity.y * dt * ysmooth;

    camera.target.x = (int)lerp(camera.target.x, ideal_x, xsmooth * dt);
    camera.target.y = (int)lerp(camera.target.y, ideal_y, ysmooth * dt);

    camera.offset.x = CANVAS_WIDTH/2;
    camera.offset.y = CANVAS_HEIGHT/2;

    const auto delta = GetMouseWheelMove();
}

void UpdatePlayer(uptr<Game>& game, entt::registry& reg) {
    auto view = reg.view<Player, Physics, Body, Character, SimpleAnimation>();

    const auto dt = GetFrameTime();

    view.each([dt, &game](auto &player, auto& physics, auto& body, auto& character, SimpleAnimation& sprite){
        const auto equiped = character.equiped;

        const auto [ax, ay] = Input::I()->GetMovementVector();
        sprite.scale.x = physics.facingX;
        physics.velocity.x += ax * 400.0f * dt;

        if (Input::I()->Jump() && physics.on_ground) {
            physics.velocity.y -= 18000.0f * dt;
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

                player.hit = std::optional{
                    SpawnPlayerHit(
                        game,
                        weaponItem,
                        body.center().x - weaponItem.region.width / 2 + weaponItem.region.width * physics.facingX,
                        body.center().y - weaponItem.region.height / 2,
                        physics.facingX)};
            }
        }
    });
}
