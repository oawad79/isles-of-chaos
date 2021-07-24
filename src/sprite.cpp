#include "sprite.hpp"

#include <variant>
#include "actor.hpp"

struct SpriteBody {
    Sprite sprite;
    Body body;
};

struct AnimSpriteBody {
    SimpleAnimation sprite;
    Body body;
};

Animation CreateUniformAnimation(Rectangle startRegion, int numFrames, float speed) {
  auto anim = Animation {  };
  anim.frames = {};

  for (int i = 0; i < numFrames; i++) {
    anim.frames.push_back({ 
      startRegion.x + startRegion.width * i,
      startRegion.y,
      startRegion.width,
      startRegion.height,
      speed
    });
  }

  return anim;
}

void LoadSpriteRenderer(SpriteRenderer& self) {
    self.target = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
}

void UpdateSprites(entt::registry& reg) {
    const float dt = GetFrameTime();
    const float time = GetTime();

    auto sim_anims = reg.view<Body, SimpleAnimation>(entt::exclude<Disabled>);
    auto adv_anims = reg.view<Body, AdvancedAnimation>(entt::exclude<Disabled>);

    auto sprites = reg.view<Body, Sprite>(entt::exclude<Disabled>);
    auto items = reg.view<Sprite, Item, Physics>(entt::exclude<Disabled>);

    const auto get_tint = [&](entt::entity ent) {
		Color tint = WHITE;
        if (reg.has<Health>(ent)) {
            auto& health = reg.get<Health>(ent);
            if (!health.canHit()) {
                tint = RED;
            } else {
                tint = WHITE;
            }
        }

        if (reg.has<Actor>(ent)) {
          auto &actor = reg.get<Actor>(ent);
          if (actor.stunTimer > BASE_STUN_TIME - 0.05f) {
            tint = YELLOW;
          }
        }

        return tint;
    };

    for (auto& ent : sprites) {
        auto& sprite = reg.get<Sprite>(ent);
		sprite.tint2 = get_tint(ent); 
		sprite.tint  = get_tint(ent); 
    }

    for (auto& ent : sim_anims) {
        auto& sprite = reg.get<SimpleAnimation>(ent); 
        sprite.tint2 = get_tint(ent);
		sprite.tint  = get_tint(ent); 

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

    for (auto& ent : adv_anims) {
        auto& sprite = reg.get<AdvancedAnimation>(ent);
        sprite.tint2 = get_tint(ent);
		sprite.tint  = get_tint(ent); 

        if (sprite.T != Type::ANIMATION || sprite.currentAnimation == "") continue;
        if (sprite.playback == Playback::FORWARD) {
            auto anim = sprite.animations[sprite.currentAnimation];
            Frame frame = anim.frames[sprite.currentFrame];

            if (sprite.time >= frame.speed / 1000.0f) {
                sprite.time = 0.0f;
                sprite.currentFrame += 1;
                sprite.currentFrame %= anim.frames.size();
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
    auto adv_anims = reg.view<Body, AdvancedAnimation>(entt::exclude<Disabled, Item>);

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
                int(body.x),
                int(body.y),
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
                {(float)int(body.x + ox + rw/2),
                 (float)int(body.y + oy + rh/2),
                 sprite.region.width,
                 sprite.region.height},
                Vector2{rw/2, rh/2},
                sprite.rotation,
                tint
            );
        }
    };

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
            { floor(body.x + ox + rw/2), floor(body.y + oy + rh/2), sw, sh },
            Vector2{rw/2, rh/2},
            sprite.rotation,
            tint
        );
    }

    for (auto& e : adv_anims) {
        auto& sprite = reg.get<AdvancedAnimation>(e);
        const auto& body = reg.get<Body>(e);

        if (sprite.currentAnimation == "") continue;

        const auto tint = Color{
          (unsigned char)((sprite.tint.r + sprite.tint2.r) / 2),
          (unsigned char)((sprite.tint.g + sprite.tint2.g) / 2),
          (unsigned char)((sprite.tint.b + sprite.tint2.b) / 2),
          (unsigned char)((sprite.tint.a + sprite.tint2.a) / 2),
        };

        const auto& animation = sprite.animations[sprite.currentAnimation];

        const auto& frame = animation.frames[sprite.currentFrame]; 

        int rw = frame.width;
        int rh = frame.height;

        const auto sw = frame.width;
        const auto sh = frame.height;

        const auto [ox, oy] = sprite.offset;
        const Rectangle rec = { body.x + ox + rw/2 + 1, body.y + oy + rh/2 + 1, sw, sh };

        DrawTexturePro(
            sprite.texture,
            {frame.x, frame.y, rw * sprite.scale.x, frame.height},
            rec,
            Vector2{frame.width/2, frame.height/2},
            sprite.rotation,
            tint
        );
    }

    for (auto e : sprites){
        const auto& sprite = reg.get<Sprite>(e);
        const auto& body = reg.get<Body>(e);
        drawSprite(body, sprite);
    }


    for (auto e : items){
        const auto& sprite = reg.get<Sprite>(e);
        const auto& body = reg.get<Body>(e);
        drawSprite(body, sprite);
    }
}
