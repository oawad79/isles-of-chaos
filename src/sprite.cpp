#include "sprite.hpp"

#include <variant>

struct SpriteBody {
    Sprite sprite;
    Body body;
};

struct AnimSpriteBody {
    SimpleAnimation sprite;
    Body body;
};

void LoadSpriteRenderer(SpriteRenderer& self) {
}

void UpdateSprites(entt::registry& reg) {
    const float dt = GetFrameTime();
    const float time = GetTime();

    auto sim_anims = reg.view<Body, SimpleAnimation>(entt::exclude<Disabled>);
    auto sprites = reg.view<Body, Sprite>(entt::exclude<Disabled>);
    auto items = reg.view<Sprite, Item, Physics>(entt::exclude<Disabled>);

    for (auto& ent : sprites) {
        auto& sprite = reg.get<Sprite>(ent);
        if (reg.has<Health>(ent)) {
            auto& health = reg.get<Health>(ent);
            if (!health.canHit()) {
                sprite.tint2 = RED;
            } else {
                sprite.tint2 = WHITE;
            }
        }
    }

    for (auto& ent : sim_anims) {
        auto& sprite = reg.get<SimpleAnimation>(ent);

        if (reg.has<Health>(ent)) {
            auto& health = reg.get<Health>(ent);
            if (!health.canHit()) {
                sprite.tint2 = RED;
            } else {
                sprite.tint2 = WHITE;
            }
        }

        if (sprite.T != Type::ANIMATION) continue;

        if (sprite.playback == Playback::FORWARD) {
            if (sprite.time >= sprite.speed / 1000.0f) {
                sprite.time = 0.0f;
                sprite.current_frame += 1;
                sprite.current_frame %= sprite.number_of_frames;
            } else {
                sprite.time += dt;
            }
        }
    }

    for (auto& ent : items) {
        auto& item = reg.get<Item>(ent);
        if (item.flags & ITEM_FLAG_BOUNCE) {
            auto& sprite = reg.get<Sprite>(ent);
            sprite.offset.y = -sprite.region.height + cosf(time*10.0f) * sprite.region.height / 2.0f;
            sprite.rotation = cosf(time*2.5f) * 25.0f;
        }
    }
}

// void DrawSprite(SpriteRenderer& self, const Sprite sprite, const Body body) {
//     self.sprites_to_draw.push_back(std::make_tuple(std::move(sprite), std::move(body)));
// }

void DrawSprites(SpriteRenderer& self, entt::registry& reg) {
    auto sprites = reg.view<Body, Sprite>(entt::exclude<Disabled, Item>);
    auto sim_anims = reg.view<Body, SimpleAnimation>(entt::exclude<Disabled, Item>);

    auto items = reg.view<Body, Sprite, Item>();

    auto drawSprite = [](const auto& body, const auto& sprite) {
        const auto tint = Color{
          (unsigned char)((sprite.tint.r + sprite.tint2.r) / 2),
          (unsigned char)((sprite.tint.g + sprite.tint2.g) / 2),
          (unsigned char)((sprite.tint.b + sprite.tint2.b) / 2),
          (unsigned char)((sprite.tint.a + sprite.tint2.a) / 2),
        };

        const auto sw = sprite.region.width;
        const auto sh = sprite.region.height;

        const auto [ox, oy] = sprite.offset;

        if (sprite.T == Type::RECTANGLE) {
            DrawRectangle(
                ceil(body.x),
                ceil(body.y),
                body.width,
                body.height,
                tint);
        }

        if (sprite.T == Type::SPRITE) {
            const auto [_rx, _ry, rw, rh] = sprite.region;
            DrawTexturePro(
                sprite.texture,
                {
                    sprite.region.x,
                    sprite.region.y,
                    sprite.region.width * sprite.scale.x,
                    sprite.region.height * sprite.scale.y,
                },
                {ceil(body.x + ox + rw/2),
                    ceil(body.y + oy + rh/2),
                    body.width,
                    body.height},
                Vector2{rw/2, rh/2},
                sprite.rotation,
                tint
            );
        }
    };

    for (auto e : sprites){
        const auto& sprite = reg.get<Sprite>(e);
        const auto& body = reg.get<Body>(e);
        drawSprite(body, sprite);
    }

    for (auto e : sim_anims) {
        const auto& sprite = reg.get<SimpleAnimation>(e);
        const auto& body = reg.get<Body>(e);
        const auto tint = Color{
          (unsigned char)((sprite.tint.r + sprite.tint2.r) / 2),
          (unsigned char)((sprite.tint.g + sprite.tint2.g) / 2),
          (unsigned char)((sprite.tint.b + sprite.tint2.b) / 2),
          (unsigned char)((sprite.tint.a + sprite.tint2.a) / 2),
        };

        const auto sw = sprite.region.width;
        const auto sh = sprite.region.height;

        const auto [_rx, _ry, rw, rh] = sprite.region;

        const auto [ox, oy] = sprite.offset;

        DrawTexturePro(
            sprite.texture,
            {
                sprite.region.x + sw * sprite.current_frame,
                sprite.region.y,
                sprite.region.width * sprite.scale.x,
                sprite.region.height * sprite.scale.y,
            },
            {
                ceil(body.x + ox + rw/2),
                ceil(body.y + oy + rh/2),
                sw,
                sh
            },
            Vector2{rw/2, rh/2},
            sprite.rotation,
            tint
        );

    }

    for (auto e : items){
        const auto& sprite = reg.get<Sprite>(e);
        const auto& body = reg.get<Body>(e);
        drawSprite(body, sprite);
    }
}
