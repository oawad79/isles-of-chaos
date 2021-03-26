#include "actor.hpp"

void UpdateZambieAi(entt::registry& reg, entt::entity self, Body& body, Physics& physics, Actor& actor) {
    switch(actor.state) {
        case ActorState::IDLE: {
            actor.target[0] = Vector2{body.center().x + -200 + RandFloat() * 400, body.y};
            actor.timer[0] = RandFloat(0.5f, 2.0f);
            actor.state = ActorState::WONDER;
            break;
        }
        case ActorState::WONDER: {
            const float dist = Vector2Distance(body.center(), actor.target[0]);

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

void UpdateGhostAi(entt::registry& reg, Body& body, Physics& physics, Actor& actor, Sprite& sprite) {
    constexpr auto TRACKING_DIST { 100.0f };

    const auto dt = GetFrameTime();

    auto players = reg.view<Player, Body>();

    float dist = 9999.0f;
    const auto angle = atan2f(
        actor.target[0].y,
        actor.target[0].x
    );

    players.each([&body, &dist](auto &player, auto& playerBody) {
        dist = Vector2Distance(body.center(), playerBody.center());
    });

    switch(actor.state) {
        case ActorState::IDLE: {
            const auto angle = RandAngle();
            actor.target[0] = Vector2{
            cosf(angle) * RandFloat(50.0f, 300.0f),
            sinf(angle) * RandFloat(50.0f, 300.0f),
            };
            actor.timer[0] = RandFloat(0.5f, 2.0f);
            actor.state = ActorState::WONDER;
            break;
        }

        case ActorState::FIND_NEW_TARGET: {
            const auto angle = atan2f(
                actor.target[0].y,
                actor.target[0].x
            ) + RandFloat(-(float)PI/4.0f, (float)PI/4.0f);

            actor.target[0] = Vector2Add(actor.target[0], Vector2{ cosf(angle), sinf(angle) });
            actor.state = ActorState::WONDER;

            break;
        }

        case ActorState::WONDER: {
            if (dist < TRACKING_DIST) {
                actor.state = ActorState::TRACKING;
                break;
            }

            const auto angle = atan2f(
                body.center().y - actor.target[0].y,
                body.center().x - actor.target[0].x
            ) - (float)(PI);

            physics.velocity.x = cosf(angle) * 1000.0f * dt;
            physics.velocity.y = sinf(angle) * 1000.0f * dt;

            float distFromTarget = Vector2Distance(body.center(), actor.target[0]);

            if (distFromTarget < 10.0f || actor.timer[0] <= 0.0f) {
                actor.state = ActorState::IDLE;
            }

            actor.timer[0] -= dt;

            break;
        }

        case ActorState::WAITING: {
            if (actor.timer[0] <= 0.0f)  {
                actor.state = ActorState::TRACKING;
                break;
            }
            actor.timer[0] -= dt;
            break;
        }

        case ActorState::TRACKING: {
            players.each([dt, dist, &actor, &body, &physics](auto &player, auto& playerBody){
                auto [x, y] = body.center();
                auto [px, py] = playerBody.center();

                float angle = atan2f(y - py, x - px) - (float)(PI);

                float speed = dist / (float)TRACKING_DIST;

                physics.velocity = {
                    cosf(angle) * 3000.0f * (1.0f + speed) * dt,
                    sinf(angle) * 3000.0f * (1.0f + speed) * dt,
                };

                if (dist <= playerBody.width) {
                    float newAngle = angle + (float)(PI) + RandFloat(-(float)PI/4.0f, (float)PI/4.0f);
                    physics.velocity = {
                    cosf(newAngle) * 6000.0f * dt,
                    sinf(newAngle) * 6000.0f * dt,
                    };
                    actor.timer[0] = RandFloat(0.5f, 2.0f);
                    actor.state = ActorState::WAITING;
                }
            });
            break;
        }
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

                        if (health.shouldDie()) {

                            // Handle loot dropping from enemies w/ loot
                            if (reg.has<Loot>(ent)) {
                                SpawnLoot(
                                    reg,
                                    reg.get<Loot>(ent),
                                    body.center(),
                                    Range1D{0, 0},
                                    Range2D{
                                        Range1D{-2000.0f, 2000.0f},
                                        Range1D{5000.0f, 8000.0f}
                                    });
                            }

                        }
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

                case ActorType::GHOST: {
                    auto& sprite = reg.get<SimpleAnimation>(ent);
                    UpdateGhostAi(reg, body, physics, actor, sprite);
                    break;
                }

                default: break;
            }
        }
    }
}
