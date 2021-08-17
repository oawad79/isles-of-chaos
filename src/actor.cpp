#include "actor.hpp"

void UpdateZambieAi(entt::registry& reg, entt::entity self, Body& body, Physics& physics, Actor& actor) {
    constexpr auto TRACKING_DIST { 80.0f };
    constexpr auto WAIT_MAX_TIME { 3.0f };
    constexpr auto WAIT_MIN_TIME { 0.8f };

    const auto dt = GetFrameTime();

    auto players = reg.view<Player, Body>();

    float distToPlayer = 9999.0f;

    float dirToPlayer = 0.0f;
    players.each([&body, &distToPlayer, &dirToPlayer](auto &player, auto& playerBody) {
        distToPlayer = Vector2Distance(body.center(), playerBody.center());
        dirToPlayer = playerBody.center().x > body.center().x ? 1 : -1;
    });

    auto& sprite = reg.get<SimpleAnimation>(self);
    
    if (distToPlayer < TRACKING_DIST && actor.state != ActorState::STUNNED) {
      actor.state = ActorState::TRACKING;
    }

    switch(actor.state) {
        case ActorState::IDLE: {
            sprite.playback = Playback::PAUSED;
            actor.target[0] = Vector2{body.center().x + -200 + RandFloat() * 400, body.y};
            actor.timer[0] = RandFloat(0.5f, 2.0f);
            actor.state = ActorState::WONDER;
            break;
        }
        case ActorState::WONDER: {
            sprite.playback = Playback::FORWARD;
            const float targetDist = Vector2Distance(body.center(), actor.target[0]);

            if (actor.target[0].x < body.center().x) {
                physics.velocity.x -= 100.0f * GetFrameTime();
            } else {
                physics.velocity.x += 100.0f * GetFrameTime();
            }

            actor.timer[0] -= GetFrameTime();
            if (targetDist < 10.0f || actor.timer[0] <= 0.0f) {
                actor.timer[0] = RandFloat(WAIT_MIN_TIME, WAIT_MAX_TIME);
                actor.state = ActorState::WAITING;
            }

            break;
        }

        case ActorState::TRACKING: {
          sprite.playback = Playback::FORWARD;
          if (distToPlayer > TRACKING_DIST * 1.2f) {
            actor.state = ActorState::IDLE;
          }

          if (RandFloat() * 100.0f > 98.0f && physics.on_ground) {
            physics.velocity.y = -12000.0f * dt;
            physics.velocity.x = dirToPlayer * 5000.0f * RandFloat(0.5f, 1.2f) * dt;
          }

          players.each([&body, &physics](auto &player, auto& playerBody){
            int dx = body.center().x < playerBody.center().x ? 1 : -1;
            physics.velocity.x += dx * 200.0f * GetFrameTime();
          });

          break;
        }

        case ActorState::WAITING: {
            sprite.playback = Playback::PAUSED;
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

void UpdateShroombaAi(entt::registry& reg, Body& body, Physics& physics, Actor& actor, Sprite& sprite) {
    constexpr auto TRACKING_DIST { 60.0f };
    constexpr auto WAIT_MAX_TIME { 3.0f };
    constexpr auto WAIT_MIN_TIME { 0.8f };

    const auto dt = GetFrameTime();

    auto players = reg.view<Player, Body>();

    float distToPlayer = 9999.0f;

    float dirToPlayer = 0.0f;
    players.each([&body, &distToPlayer, &dirToPlayer](auto &player, auto& playerBody) {
        distToPlayer = Vector2Distance(body.center(), playerBody.center());
        dirToPlayer = playerBody.center().x > body.center().x ? 1 : -1;
    }); 

    switch(actor.state) {
        case ActorState::IDLE: {
            actor.target[0] = Vector2{body.center().x + -200 + RandFloat() * 400, body.y};
            actor.timer[0] = RandFloat(0.35f, 1.00f);
            actor.state = ActorState::WAITING;
            break;
        }
        case ActorState::WAITING: { 
            if (actor.timer[0] <= 0.0f) {
                actor.state = ActorState::JUMPING; 
                if (distToPlayer < TRACKING_DIST) {
                  physics.velocity.y = -RandFloat(10000.0f, 18000.0f) * dt;
                  physics.velocity.x = RandFloat(7000.0f, 9000.0f) * dt * dirToPlayer;
                } else { 
                  physics.velocity.y = -RandFloat(8000.0f, 13000.0f) * dt;
                  physics.velocity.x = RandFloat(7000.0f, 9000.0f) * dt * RandFloat(-1.0f, 1.0f);
                }
                physics.friction = 0.105f;
                physics.on_ground = false;
            }

            actor.timer[0] -= dt; 
            break;
        }
        case ActorState::JUMPING: {
          sprite.region.x = 48;
          if (physics.on_ground) {
              sprite.region.x = 32;
              actor.state = ActorState::IDLE;
          }
          break;
        }
        default: break;
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
            players.each([dt, dist, &actor, &body, &physics, TRACKING_DIST](auto &player, auto& playerBody){
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

void UpdateKiwiBirdAi(entt::registry& reg, const entt::entity& self) {
    const auto dt = GetFrameTime();

    auto& actor = reg.get<Actor>(self);
    auto& body = reg.get<Body>(self);
    auto& physics = reg.get<Physics>(self);
    auto& sprite = reg.get<AdvancedAnimation>(self);

    float dist = 9999.0f;
    float angleToPlayer = 0.0f;
    Vector2 playerTargetPos = {0, 0};
    Vector2 playerCenter = {0, 0};
    float playerSpeed = 0.0f;

    sprite.scale.x = physics.facingX;

    auto players = reg.view<Player, Body, Physics>();
    players.each([&](auto &player, auto& playerBody, auto& physics) {
        const auto a = body.center();
        auto b = playerBody.center();
        b.y -= playerBody.height * 2;

        dist = Vector2Distance(a, b);
        angleToPlayer = atan2f(b.y - a.y, b.x - a.x) - PI/4;
        playerTargetPos = b;
        playerSpeed = Vector2Length(physics.velocity);
        playerCenter = playerBody.center();
    });

    switch(actor.state) {
        case ActorState::IDLE: {
            actor.state = ActorState::TRACKING;
            break;
        }

        case ActorState::WONDER: {
            actor.target[0].x += GetFrameTime() * 1.2f;
            float angle = actor.target[0].x;
            auto vel = Vector2{
                cosf(angle) * 2.3f,
                sinf(angle + cosf(angle / 3.0f) * 0.5f)
            };
            physics.velocity.x += vel.x * 50.0f * GetFrameTime();
            physics.velocity.y += vel.y * 50.0f * GetFrameTime();
            if (dist > 50) { actor.state = ActorState::TRACKING; }
            // TODO(Dustin): check if player vel is zero
            if (actor.timer[0] <= 0.0f && playerSpeed <= 0.1f) {
                actor.state = ActorState::LOOKING_TO_SIT;
            }
            actor.timer[0] -= GetFrameTime();
            break;
        }

        case ActorState::LOOKING_TO_SIT: {
            if (playerSpeed > 0.25f) {
                actor.state = ActorState::TRACKING;
                break;
            }

            Vector2 t = playerCenter;
            t.x -= 2;
            t.y -= 7;

            float d = Vector2Distance(t, body.center());

            float angle = atan2f(t.y - body.center().y, t.x - body.center().x) - PI/4;

            auto vel = Vector2{
                cosf(angle),
                sinf(angle)
            };

            vel.x *= 3500.0f * dt;
            vel.y *= 2500.0f * dt;
            physics.velocity = vel;

            if (d <= 1.0) {
                physics.velocity.x = 0;
                physics.velocity.y = 0;
                body.x = t.x - body.width / 2.0f;
                body.y = t.y - body.height / 2.0f;
                actor.state = ActorState::SITTING;
            }

            break;
        }

        case ActorState::SITTING: {
            if (playerSpeed > 0.25f) {
                actor.state = ActorState::TRACKING;
                break;
            }
            break;
        }

        case ActorState::TRACKING: {
            auto vel = Vector2{
                cosf(angleToPlayer),
                sinf(angleToPlayer)
            };
            if (dist > 20) {
                vel.x *= (4200.0f + dist * 4) * dt;
                vel.y *= (2500.0f + dist * 4) * dt;
                physics.velocity = vel;
            } else {
                actor.state = ActorState::WONDER;
                actor.target[0].x = atan2f(vel.y, vel.x) - M_PI / 2;
                actor.timer[0] = 4.0f;
            }

            break;
        }

        default:
            break;
    }
}

void UpdateBug(entt::registry& reg, const entt::entity& self) {
  auto& actor = reg.get<Actor>(self);
  auto& body = reg.get<Body>(self);
  auto& physics = reg.get<Physics>(self);
  auto& sprite = reg.get<SimpleAnimation>(self);

  auto playerBody = Body{0};

  reg.view<Body, Player>().each([&](auto& body, auto& _p){
    playerBody = body;
  });

  constexpr auto speed = 20;

  switch (actor.state) {
    case ActorState::IDLE: {
      actor.target[0] = Vector2{body.center().x + -100 + RandFloat() * 200, body.center().y};
      actor.timer[0] = RandFloat(0.2f, 2.0f);
      actor.state = ActorState::WONDER;
      break;
    }
    case ActorState::WONDER: {
      physics.velocity.x += (1.0f + (0.5f * cosf(GetTime()))) * (body.center().x > actor.target[0].x ? -1 : 1) * speed * GetFrameTime();
      if (Vector2Distance(body.center(), actor.target[0]) < body.width / 2.f || actor.timer[0] <= 0.0f) {
        actor.state = ActorState::IDLE;
      }
      actor.timer[0] -= GetFrameTime();
      break;
    }
    case ActorState::DEAD: {
      physics.velocity.x = 0;
      sprite.playback = Playback::PAUSED;
      sprite.current_frame = 5;
      break;
    }
    default:
      break;
  }

  if (CheckCollisionRecs(playerBody, body)) {
    actor.state = ActorState::DEAD;
  }
}

bool IsEnemy(Actor actor) {
  return actor.type > ActorType::ENEMY_START 
    && actor.type < ActorType::ENEMY_END;
}

void Stun(Actor& actor) {
  actor.stunTimer = BASE_STUN_TIME;
  actor.last = actor.state;
  actor.state = ActorState::STUNNED;
}

void UpdateActor(entt::registry& reg) {
    bool playerExists = false;
    Body playerBody = Body{0};

    Player* playerState = NULL;

    reg.view<Player,Body>().each([&playerBody, &playerExists, &playerState](auto& player, Body &body){
      playerBody = body;
      playerExists = true;
      playerState = &player;
    });

    auto view = reg.view<Actor, Physics, Body>(entt::exclude<Disabled>);

    for (auto& ent : view) {
        auto& actor = reg.get<Actor>(ent);
        auto& body = reg.get<Body>(ent);
        auto& physics = reg.get<Physics>(ent);

        if (physics.velocity.x > 0) physics.facingX = RIGHT;
        else physics.facingX = LEFT;

        if (actor.type > ActorType::ENEMY_START && actor.type < ActorType::ENEMY_END && actor.state != ActorState::STUNNED) {
            reg.view<PlayerHit, Body, Item>().each([&](auto& phit, auto& obody, auto& item){
                const auto damage = item.effectValue;
                if (CheckCollisionRecs(body, obody)) {
                    const auto dir = body.center().x > playerBody.center().x ? 1 : -1;
                    if (reg.has<Health>(ent)) {
                        auto& health = reg.get<Health>(ent);

                        if (health.canHit()) {
                            SpawnHitParticles(reg, body.center());
                            physics.velocity.x = 40 * dir;
                            physics.velocity.y = 90 * dir;
                            Stun(actor);
                        }

                        health.hit(damage);

                        if (health.shouldDie()) {

                            // Giving the player an extra attack after killing a monster
                            // @MECHANIC
                            playerState->attackCooloff = 0.0f;

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
        }

      if (actor.state == ActorState::STUNNED) {
          if (actor.stunTimer > 0.0f)  {
            actor.stunTimer -= GetFrameTime();
          } else {
            actor.stunTimer = 0.0f;
            auto tmp = actor.state;
            actor.state = actor.last;
            actor.last = tmp;
          };
        }

        switch (actor.type) {
            case ActorType::KIWI_BIRD: {
                UpdateKiwiBirdAi(reg, ent);
                break;
            }
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
            case ActorType::SHROOMBA: {
                auto& sprite = reg.get<Sprite>(ent);
                UpdateShroombaAi(reg, body, physics, actor, sprite);
                break;
            }
            case ActorType::BUG: {
                UpdateBug(reg, ent);
                break;
            }
            default: break;
        }
    }
}
