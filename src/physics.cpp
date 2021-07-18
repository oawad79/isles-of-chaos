#include "physics.hpp"
#include "player.hpp"

constexpr float GRAVITY {1000.0f};
constexpr float MAX_GRAVITY {200.0f};

float IntervalDistance(float min_a, float max_a, float min_b, float max_b) {
    if (max_a < min_b) return min_a - max_a;
    else return min_a - max_b;
}

std::tuple<bool, Vector2>
LineIntersection(Vector2 a1, Vector2 a2, Vector2 b1, Vector2 b2) {
    Vector2 intersection = Vector2{0.0f, 0.0f};
    const Vector2 b = Vector2Subtract(a2, a1);
    const Vector2 d = Vector2Subtract(b2, b1);
    const float b_dot_perp = b.x * d.y - b.y * d.x;
    if (b_dot_perp == 0) return {false, {0, 0}};
    const Vector2 c = Vector2Subtract(b1, a1);
    const float t = (c.x * d.y - c.y * d.x) / b_dot_perp;
    if (t < 0.0f || t > 1.0f) return {false, {0, 0}};
    const float u = (c.x * b.y - c.y * b.x) / b_dot_perp;
    if (u < 0.0f || u > 1.0f) return {false, {0, 0}};
    intersection = Vector2Add(a1, Vector2{b.x * t, b.y * t});
    return {true, intersection};
}

std::tuple<bool, Vector2>
CheckCollisionRecPoly(const Rectangle& r, const Polygon& poly) {
    const Line left_wall = {{r.x, r.y}, {r.x, r.y + r.height}};
    const Line right_wall = {{r.x+r.width, r.y}, {r.x+r.width, r.y+r.height}};
    const Line top_wall = {{r.x, r.y}, {r.x+r.width, r.y}};
    const Line bottom_wall = {{r.x, r.y+r.height}, {r.x+r.width, r.y+r.height}};

    for (size_t i = 0; i < poly.points.size(); i++) {
        Vector2 a = poly.points[i];
        Vector2 b;

        if (i >= poly.points.size() - 1) {
            b = poly.points[0];
        } else {
            b = poly.points[i + 1];
        }

        a.x += poly.x + poly.offset.x;
        a.y += poly.y + poly.offset.y;
        b.x += poly.x + poly.offset.x;
        b.y += poly.y + poly.offset.y;

        const auto a1 = LineIntersection(left_wall.a, left_wall.b, a, b);
        const auto a2 = LineIntersection(right_wall.a, right_wall.b, a, b);
        const auto a3 = LineIntersection(top_wall.a, top_wall.b, a, b);
        const auto a4 = LineIntersection(bottom_wall.a, bottom_wall.b, a, b);

        if (std::get<0>(a1)) return a1;
        if (std::get<0>(a2)) return a2;
        if (std::get<0>(a3)) return a3;
        if (std::get<0>(a4)) return a4;
    }

    return {false, Vector2{0, 0}};
}

void SetOnGround(Physics& phys) {
    phys.on_ground = true;
    phys.on_ground_timer = 0.1f;
}

void UpdatePhysics(uptr<Game>& game, entt::registry& reg) {
    if (game->physicsPaused) return;

    auto view = reg.view<Physics, Body>(entt::exclude<Disabled>);

    const float dt = GetFrameTime();

    const auto* tilemap = GetTilemap(game->level);

    static float high = 0.0f;

    for (auto& e : view) {
        auto& physics = reg.get<Physics>(e);
        auto& body = reg.get<Body>(e);

        if (!physics.on_ladder) {
            physics.velocity.y += GRAVITY * dt * physics.gravityScale.y;
            if (physics.velocity.y > MAX_GRAVITY) physics.velocity.y = MAX_GRAVITY;
        }

        Rectangle xbody {body.x + physics.velocity.x * dt, body.y, body.width, body.height};
        Rectangle ybody {body.x, body.y + physics.velocity.y * dt, body.width, body.height};
        Rectangle ybody_half_vel {body.x, body.y + (physics.velocity.y * 0.25f) * dt, body.width, body.height};

      physics.colliding_with_solid = false;

        if (physics.on_ground_timer <= 0)
            physics.on_ground = false;
        else
            physics.on_ground_timer -= GetFrameTime();

        physics.on_ladder = false;

        if (tilemap != nullptr) {
            for (const auto& poly: tilemap->geometry) {
                if (poly.type == SolidType::Polygon) {
                    const auto [xcoll, xwhere] = CheckCollisionRecPoly(xbody, poly);
                    const auto [ycoll, ywhere] = CheckCollisionRecPoly(ybody, poly);

                    if (ycoll && physics.type != PhysicsType::KINEMATIC) {
                        ybody = body;
                        if (ybody.y + ybody.height / 2 < ywhere.y) {
                            SetOnGround(physics);
                            ybody.y = ywhere.y - ybody.height;
                            physics.solid_collision_point_left = ywhere;
                            physics.velocity.y = 0;
                        }
                    }

                    if (xcoll || ycoll) {
                        physics.colliding_with_solid = true;
                        physics.solid_collision_point = xcoll?xwhere:ywhere;
                    }
                } else {
                    const auto xcoll = CheckCollisionRecs(xbody, poly.bounds());
                    auto ycoll = CheckCollisionRecs(ybody, poly.bounds());
                    ycoll = ycoll || CheckCollisionRecs(ybody_half_vel, poly.bounds());

                    if (xcoll && poly.height != 0 && physics.type != PhysicsType::KINEMATIC) {
                        const float depth = body.y + body.height - poly.bounds().y;

                        if (poly.type == SolidType::Rectangle && depth >= 2.0f) {
                            xbody = body;
                            physics.velocity.x = 0.0f;
                        }
                    }

                    if (ycoll && physics.type != PhysicsType::KINEMATIC) {
                        const float depth = body.y + body.height - poly.bounds().y;

                        if (poly.height == 0) {
                            if (poly.bounds().y + poly.height > body.y + body.height - 1) {
                                ybody = body;
                                physics.velocity.y = 0.0f;
                                SetOnGround(physics);
                            }
                        } else {
                            if (poly.bounds().y + poly.height > body.y + body.height - 1) {
                                ybody = body;
                                physics.velocity.y = 0.0f;
                                SetOnGround(physics);
                                if (depth > 0) ybody.y -= 1;
                            } else {
                                physics.velocity.y = -physics.velocity.y * 0.25f;
                                ybody.y = poly.bounds().y + poly.height;
                            }
                        }
                    }

                    if (xcoll || ycoll) physics.colliding_with_solid = true;
                }
            }

            physics.on_ladder = false;
            for (const auto& obj : tilemap->features) {
                if (obj.type == FeatureType::Ladder) {
                    if (CheckCollisionRecs(body, obj.bounds())) {
                        if (body.y + body.height < obj.y + obj.height) {
//                            const auto depth = (ybody.y + body.height) - obj.y;
//                            if (depth < 1.0f) {
//                                SetOnGround(physics);
//                                physics.velocity.y = 0.0f;
//                                ybody.y -= depth;
//                            }
                        }

                        physics.on_ladder = true;
                    }
                }
            }
        }

        body.x = xbody.x;
        body.y = ybody.y;

        physics.velocity.x *= std::pow(physics.friction, dt);
        physics.velocity.y *= std::pow(physics.friction, dt);
    }
}

void DrawDebugPhysicsInfo(const uptr<Game>& game, entt::registry& reg) {
    const auto* tilemap = GetTilemap(game->level);
    for (const auto& poly: tilemap->geometry) {
        if (poly.type == SolidType::Polygon) {
            for (size_t i = 0; i < poly.points.size(); i++) {
                Vector2 a = poly.points[i];
                Vector2 b;

                if (i >= poly.points.size() - 1) {
                    b = poly.points[0];
                } else {
                    b = poly.points[i + 1];
                }

                a.x += poly.x + poly.offset.x;
                a.y += poly.y + poly.offset.y;
                b.x += poly.x + poly.offset.x;
                b.y += poly.y + poly.offset.y;

                DrawLineEx(a, b, 1, RED);
            }
        } else {
            auto body = poly.bounds();
            if (body.height == 0) body.height += 1;
            DrawRectangleLinesEx(body, 1.0f, body.height == 1 ? YELLOW : RED);
        }
    }

    for (const auto& obj : tilemap->features) {
        auto text = std::string{"None"};
        Color color = WHITE;

        if (obj.type == FeatureType::Ladder) {
            color = YELLOW;
            text = "Ladder";
        }

        if (obj.type == FeatureType::Port) {
            color = BLUE;
            text = "Port:" + obj.target;
        }

        if (obj.type == FeatureType::Banner) {
          color = LIGHTGRAY;
          text = "Banner";
        }

        const auto b = obj.bounds();

        DrawRectangleLines(b.x, b.y, b.width, b.height, color);
        DrawText(text.c_str(), b.x, b.y - 9, 4, color);
    }

    auto view = reg.view<Physics, Body>(entt::exclude<Disabled>);
    for (auto e : view) {
        auto& physics = reg.get<Physics>(e);
        auto& body = reg.get<Body>(e);

        DrawRectangleLinesEx(body, 1, physics.colliding_with_solid?RED:GREEN);
        DrawCircle(physics.solid_collision_point.x, physics.solid_collision_point.y, 2, YELLOW);

        DrawCircle(physics.solid_collision_point_left.x, physics.solid_collision_point_left.y, 2, BLUE);
        DrawLine(physics.solid_collision_point_left.x, physics.solid_collision_point_left.y - 8, physics.solid_collision_point_left.x, physics.solid_collision_point_left.y, BLUE);

        DrawCircle(physics.solid_collision_point_right.x, physics.solid_collision_point_right.y, 2, BLUE);
        DrawLine(physics.solid_collision_point_right.x, physics.solid_collision_point_right.y - 8, physics.solid_collision_point_right.x, physics.solid_collision_point_right.y, BLUE);

        if (physics.on_ground) {
            DrawLine(
                body.x,
                body.y+body.height-1,
                body.x+body.width-1,
                body.y+body.height-1, BLUE);
        }
    }

    auto view2 = reg.view<Body, PlayerHit>(entt::exclude<Disabled>);
    for (auto e : view2) {
      auto& body = reg.get<Body>(e);
      DrawRectangleLinesEx(body, 1, BLUE);
    }
}
