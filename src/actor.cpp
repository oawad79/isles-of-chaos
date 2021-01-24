#include "actor.hpp"

void UpdateZambieAi(entt::registry& reg, entt::entity self, Body& body, Physics& physics, Actor& actor) {
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

    auto& spr = reg.get<SimpleAnimation>(self);
    spr.scale.x = physics.facingX;
    if (std::abs(physics.velocity.x) < 0.1f) {
        spr.playback = Playback::PAUSED;
        spr.current_frame = 0;
    } else {
        spr.playback = Playback::FORWARD;
    }
}

void UpdateDreadSharkAi(entt::registry& reg, Body& body, Physics& physics, Actor& actor, Sprite& sprite) {
    constexpr auto JUMP_HEIGHT {160.0f};
    switch(actor.state) {
        case ActorState::IDLE: {
            body.y += JUMP_HEIGHT;
            actor.target[0].y = body.y;
            actor.state = ActorState::JUMP;
            break;
        }
        case ActorState::JUMP: {
            physics.velocity.y = -JUMP_HEIGHT * (1.0f/GetFrameTime())/10.0f;
            actor.state = ActorState::JUMPING;
            break;
        }
        case ActorState::JUMPING: {
            if (body.y > actor.target[0].y + 4.0f) {
                body.y = actor.target[0].y - 4.0f;
                physics.velocity.y = 0.0f;
                physics.gravityScale.y = 0.0f;
                actor.state = ActorState::WAITING;
                actor.timer[0] = 1.0f;
            }
            break;
        }
        case ActorState::WAITING: {

            if (actor.timer[0] <= 0) {
                physics.gravityScale.y = 1.0f;
                actor.state = ActorState::JUMP;
            } else {
                actor.timer[0] -= GetFrameTime();
            }

            break;
        }
        default: break;
    }

    if (physics.velocity.y < 0.0f) {
        sprite.rotation = 0.0f;
    } else {
        sprite.rotation = lerp(sprite.rotation, -180.0f, 8.0f * GetFrameTime());
    }

}

void UpdateActor(entt::registry& reg) {
    auto view = reg.view<Actor, Physics, Body>(entt::exclude<Disabled>);

    for (auto& ent : view) {
        auto& actor = reg.get<Actor>(ent);
        auto& body = reg.get<Body>(ent);
        auto& physics = reg.get<Physics>(ent);

        if (physics.velocity.x > 0)
            physics.facingX = RIGHT;
        else
            physics.facingX = LEFT;

        if (actor.type > ActorType::ENEMY_START && actor.type < ActorType::ENEMY_END) {
            reg.view<PlayerHit, Body, Item>().each([&](auto& phit, auto& obody, auto& item){
                const auto damage = item.effectValue;
                if (CheckCollisionRecs(body, obody)) {
                    const auto dir = body.center().x > obody.center().x ? 1 : -1;
                    if (reg.has<Health>(ent)) {
                        auto& health = reg.get<Health>(ent);

                        if (health.canHit()) {
                            SpawnHitParticles(reg, body.center());
                            physics.velocity.x = 4000 * dir * GetFrameTime();
                            physics.velocity.y = 9000 * dir * GetFrameTime();
                        }

                        health.hit(damage);
                    }
                }
            });

            switch (actor.type) {
                case ActorType::ZAMBIE: {
                    UpdateZambieAi(reg, ent,  body, physics, actor);
                    break;
                }
                case ActorType::DREAD_SHARK: {
                    auto& sprite = reg.get<SimpleAnimation>(ent);
                    UpdateDreadSharkAi(reg, body, physics, actor, sprite);
                    break;
                }
                default: break;
            }
        }
    }
}
