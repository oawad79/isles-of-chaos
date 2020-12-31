#include "sprite.hpp"

void LoadSpriteRenderer(SpriteRenderer& self) {
}

void UpdateSprites(entt::registry& reg) {
    const float dt = GetFrameTime();

    auto sim_anims = reg.view<Body, SimpleAnimation>();

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

    std::sort(deffered_sprites.begin(), deffered_sprites.end(),
    [](auto& a, auto &b){
        const auto& [abody, _1] = a;
        const auto& [bbody, _2] = b;
        return abody.center().y < bbody.center().y;
    });

    // Shader shader = Assets::I()->shaders[SHADER_CHARACTERS];
    // BeginShaderMode(shader);

    for (const auto& [body, deff] : deffered_sprites) {
        if (deff->T == Type::ANIMATION) {
            const auto* sprite = dynamic_cast<SimpleAnimation*>(deff);
            const auto sw = sprite->region.width;
            const auto sh = sprite->region.height;

            DrawTexturePro(
                sprite->texture,
                {
                    sprite->region.x + sw * sprite->current_frame,
                    sprite->region.y,
                    sprite->region.width,
                    sprite->region.height,
                },
                Rectangle{body.x - sw / 4, body.y - sh + body.height, sw, sh},
                Vector2{0, 0},
                0.0f,
                sprite->tint
            );

        } else {
            const auto* sprite = deff;
            const auto sw = sprite->region.width;
            const auto sh = sprite->region.height;

            if (sprite->T == Type::RECTANGLE) {
                DrawRectangle(body.x, body.y, body.width, body.height, sprite->tint);
            }

            if (sprite->T == Type::SPRITE) {
                DrawTexturePro(
                    sprite->texture,
                    sprite->region,
                    body,
                    Vector2{0, 0},
                    0.0f,
                    sprite->tint
                );
            }
        }
    }

    self.sprites_to_draw.clear();

    // EndShaderMode();
}
