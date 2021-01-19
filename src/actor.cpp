#include "actor.hpp"

void UpdateZambieAi(entt::registry& reg, Body& body, Physics& physics, Actor& actor) {
    switch(actor.state) {
        case ActorState::IDLE: {
            actor.target[0] = Vector2{body.x + -200 + RandFloat() * 400, body.y};
            actor.timer[0] = RandFloat(0.5f, 2.0f);
            actor.state = ActorState::WONDER;
            break;
        }
        case ActorState::WONDER: {
            const float dist = Vector2Distance({body.x, body.y}, actor.target[0]);

            if (actor.target[0].x < body.center().x) {
                physics.velocity.x -= 100.0f * GetFrameTime();
            } else {
                physics.velocity.x += 100.0f * GetFrameTime();
            }

            actor.timer[0] -= GetFrameTime();
            if (dist < 10.0f || actor.timer[0] <= 0.0f) {
                actor.timer[0] = RandFloat(0.2f, 1.0f);
                actor.state = ActorState::WAITING;
            }

            break;
        }
        case ActorState::WAITING: {
            actor.timer[0] -= GetFrameTime();
            if (actor.timer[0] <= 0.0f) {
                actor.state = ActorState::IDLE;
            }
            break;
        }
        default:
            break;
    }

    reg.view<Player, Body, Health>()
        .each([&](auto& player, auto& pbody, auto& health){

    });
}

void UpdateActor(entt::registry& reg) {
    auto view = reg.view<Actor, Physics, Body>(entt::exclude<Disabled>);

    for (auto& ent : view) {
        auto& actor = reg.get<Actor>(ent);
        auto& body = reg.get<Body>(ent);
        auto& physics = reg.get<Physics>(ent);

        if (actor.type > ActorType::ENEMY_START && actor.type < ActorType::ENEMY_END) {
            reg.view<PlayerHit, Body, Item>().each([&](auto& phit, auto& obody, auto& item){
                const auto damage = item.effectValue;
                if (CheckCollisionRecs(body, obody)) {
                    if (reg.has<Health>(ent)) {
                        auto& health = reg.get<Health>(ent);
                        health.hit(damage);
                    }
                }
            });

            switch (actor.type) {
                case ActorType::ZAMBIE: {
                    UpdateZambieAi(reg, body, physics, actor);
                    break;
                }
                default: break;
            }
        }
    }
}
