#include "physics.hpp"

constexpr float GRAVITY {0.0f};

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
    Line left_wall = {{r.x, r.y}, {r.x, r.y + r.height}};
    Line right_wall = {{r.x+r.width, r.y}, {r.x+r.width, r.y+r.width}};
    Line top_wall = {{r.x, r.y}, {r.x+r.width, r.y}};
    Line bottom_wall = {{r.x, r.y+r.height}, {r.x+r.width, r.y+r.height}};

    for (size_t i = 0; i < poly.points.size(); i++) {
        Vector2 a = poly.points[i];
        Vector2 b;

        if (i >= poly.points.size() - 1) {
            b = poly.points[0];
        } else {
            b = poly.points[i + 1];
        }

        a.x += poly.position.x;
        a.y += poly.position.y;
        b.x += poly.position.x;
        b.y += poly.position.y;

        auto a1 = LineIntersection(left_wall.a, left_wall.b, a, b);
        auto a2 = LineIntersection(right_wall.a, right_wall.b, a, b);
        auto a3 = LineIntersection(top_wall.a, top_wall.b, a, b);
        auto a4 = LineIntersection(bottom_wall.a, bottom_wall.b, a, b);

        if (std::get<0>(a1)) return a1;
        if (std::get<0>(a2)) return a2;
        if (std::get<0>(a3)) return a3;
        if (std::get<0>(a4)) return a4;
    }

    return {false, Vector2{0, 0}};
}

void UpdatePhysics(uptr<Game>& game, entt::registry& reg) {
    auto view = reg.view<Physics, Body>();

    const float dt = GetFrameTime();

    for (auto& e : view) {
        auto& physics = reg.get<Physics>(e);
        auto& body = reg.get<Body>(e);

        if (!physics.on_ladder) {
            physics.velocity.y += GRAVITY * dt;
        }

        Rectangle xbody {body.x + physics.velocity.x * dt, body.y, body.width, body.height};
        Rectangle ybody {body.x, body.y + physics.velocity.y * dt, body.width, body.height};

        physics.colliding_with_solid = false;

        if (game->tilemap.get() != nullptr) {
            for (const auto& poly: game->tilemap->geometry) {
                const auto [xcoll, xwhere] = CheckCollisionRecPoly(xbody, poly);
                const auto [ycoll, ywhere] = CheckCollisionRecPoly(ybody, poly);

                if (xcoll) {
                    xbody = body;
                }

                if (ycoll) {
                    ybody = body;
                    physics.velocity.y = 0;
                }

                if (xcoll || ycoll) {
                    physics.colliding_with_solid = true;
                    physics.solid_collision_point = xcoll?xwhere:ywhere;
                }
            }

            physics.on_ladder = false;
            for (const auto& obj : game->tilemap->objects) {
                if (obj.type == "Ladder") {
                    if (CheckCollisionRecs(body, obj)) {
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

void DrawDebugPhysicsInfo(uptr<Game>& game, entt::registry& reg) {
    for (const auto& poly: game->tilemap->geometry) {
        for (size_t i = 0; i < poly.points.size(); i++) {
            Vector2 a = poly.points[i];
            Vector2 b;

            if (i >= poly.points.size() - 1) {
                b = poly.points[0];
            } else {
                b = poly.points[i + 1];
            }

            a.x += poly.position.x;
            a.y += poly.position.y;

            b.x += poly.position.x;
            b.y += poly.position.y;

            DrawLineEx(a, b, 1, RED);
        }
    }

    for (const auto& obj : game->tilemap->objects) {
        Color color = RAYWHITE;

        if (obj.type == "Ladder") {
            color = YELLOW;
        }

        DrawRectangleLines(obj.x, obj.y, obj.width, obj.height, color);
        DrawText(obj.type.c_str(), obj.x, obj.y - 9, 8, color);
    }

    auto view = reg.view<Physics, Body>();
    for (auto e : view) {
        auto& physics = reg.get<Physics>(e);
        auto& body = reg.get<Body>(e);

        DrawRectangleLinesEx(body, 1, physics.colliding_with_solid?RED:GREEN);
        DrawCircle(physics.solid_collision_point.x, physics.solid_collision_point.y, 2, YELLOW);
    }
}
