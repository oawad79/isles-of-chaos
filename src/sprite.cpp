#include "sprite.hpp"

void LoadSpriteRenderer(SpriteRenderer& self) {
}

void UpdateSprites(entt::registry& reg) {
    const float dt = GetFrameTime();
    const float time = GetTime();

    auto sim_anims = reg.view<Body, SimpleAnimation>();
    auto items = reg.view<Sprite, Item>();

    for (auto& ent : sim_anims) {
        auto& sprite = reg.get<SimpleAnimation>(ent);

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
        auto& sprite = reg.get<Sprite>(ent);
        sprite.offset.y = -sprite.region.height + cosf(time*10.0f) * sprite.region.height / 2.0f;
        sprite.rotation = cosf(time*2.5f) * 25.0f;
    }
}

void DrawSprite(SpriteRenderer& self, const Sprite sprite, const Body body) {
    self.sprites_to_draw.push_back(std::make_tuple(std::move(sprite), std::move(body)));
}

void DrawSprites(SpriteRenderer& self, entt::registry& reg) {
    auto sprites = reg.view<Body, Sprite>();
    auto sim_anims = reg.view<Body, SimpleAnimation>();

    std::vector<std::tuple<Body, Sprite*>> deffered_sprites;

    sprites.each([&deffered_sprites](auto& b, auto& s) {
        deffered_sprites.push_back({b, &s});
    });

    sim_anims.each([&deffered_sprites](auto& b, auto& s) {
        deffered_sprites.push_back({b, &s});
    });

    for (auto& [spr, body] : self.sprites_to_draw) {
        deffered_sprites.push_back({body, &spr});
    }

    for (const auto& [body, deff] : deffered_sprites) {
        if (deff->T == Type::ANIMATION) {
            const auto* sprite = dynamic_cast<SimpleAnimation*>(deff);
            const auto sw = sprite->region.width;
            const auto sh = sprite->region.height;

            const auto [ox, oy] = sprite->offset;

            DrawTexturePro(
                sprite->texture,
                {
                    sprite->region.x + sw * sprite->current_frame,
                    sprite->region.y,
                    sprite->region.width,
                    sprite->region.height,
                },
                {
                    ox + body.x - sw / 4,
                    oy + body.y - sh + body.height,
                    sw,
                    sh
                },
                Vector2{0, 0},
                0.0f,
                sprite->tint
            );

        } else {
            const auto* sprite = deff;
            const auto sw = sprite->region.width;
            const auto sh = sprite->region.height;

            const auto [ox, oy] = sprite->offset;

            if (sprite->T == Type::RECTANGLE) {
                DrawRectangle(
                    ceil(body.x),
                    ceil(body.y),
                    body.width,
                    body.height,
                    sprite->tint);
            }

            if (sprite->T == Type::SPRITE) {
                const auto [_rx, _ry, rw, rh] = sprite->region;
                DrawTexturePro(
                    sprite->texture,
                    sprite->region,
                    {ceil(body.x + ox + rw/2),
                     ceil(body.y + oy + rh/2),
                     body.width,
                     body.height},
                    Vector2{rw/2, rh/2},
                    sprite->rotation,
                    sprite->tint
                );
            }
        }
    }

    self.sprites_to_draw.clear();
}
