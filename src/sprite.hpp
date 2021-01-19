#pragma once

#include <iostream>
#include <vector>
#include <raylib.h>
#include <tuple>
#include <entt.hpp>

#include "enttypes.hpp"
#include "item.hpp"
#include "physics_type.hpp"
#include "body.hpp"
#include "utils.hpp"
#include "character.hpp"
#include "assets.hpp"

enum Type {
    RECTANGLE,
    CIRCLE,
    TEXTURE,
    SPRITE,
    ANIMATION,
};

enum Playback {
    FORWARD,
    BACKWARDS,
    FORWARD_AND_BACK,
    LOOP,
    PAUSED,
};

struct Sprite {
    Type T {Type::RECTANGLE};
    Texture2D texture;
    Rectangle region;
    Vector2 offset{0, 0};
    Vector2 scale{1, 1};
    float rotation{0};
    Color tint{WHITE};
    Color tint2{WHITE};

    virtual ~Sprite() = default;
};

struct SimpleAnimation : Sprite {
    Playback playback {Playback::FORWARD};
    int number_of_frames {1};
    int current_frame {0};
    float speed {200.0f}; // MS
    float time {0.0f};
};

struct SpriteRenderer {
    Shader shader;

    std::vector<std::tuple<Sprite, Body>> sprites_to_draw{};
};

void LoadSpriteRenderer(SpriteRenderer& self);

void DrawSprite(SpriteRenderer& self, const Sprite sprite, const Body body);

void UpdateSprites(entt::registry& reg);
void DrawSprites(SpriteRenderer& ren, entt::registry& reg);
