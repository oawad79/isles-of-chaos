#include "player.hpp"
#include <iostream>

entt::entity SpawnPlayer(entt::registry& reg, float x, float y) {
    auto self = reg.create();

    auto& body = reg.emplace<Body>(self);
    body.x = x;
    body.y = y;
    body.width = 8;
    body.height = 12;

    auto& spr = reg.emplace<Sprite>(self);
    spr.T = Type::RECTANGLE;
    spr.tint = WHITE;

    auto& player = reg.emplace<Player>(self);
    auto& physics = reg.emplace<Physics>(self);

    return self;
}

void UpdatePlayer(entt::registry& reg) {
    auto view = reg.view<Player, Physics, Body>();

    const auto dt = GetFrameTime();

    view.each([dt](auto &player, auto& physics, auto& body){
        const auto [ax, ay] = Input::I()->GetMovementVector();
        physics.velocity.x += ax * 1000.0f * dt;
        physics.velocity.y += ay * 1000.0f * dt;
    });
}
