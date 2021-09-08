#include "art.hpp"

void Art::drawRect(float x, float y, float w, float h, Color color, float layer) {
    auto s = Shape{};

    s.x = x;
    s.y = y;
    s.width = w;
    s.height = h;
    s.color = color;
    s.layer = layer;

    shapes.emplace_back(std::move(s));
}

void Art::drawRectL(float x, float y, float w, float h, Color color, float layer) {
    auto s = Shape{};

    s.x = x;
    s.y = y;
    s.type = LINE_RECT;
    s.width = w;
    s.height = h;
    s.color = color;
    s.layer = layer;

    shapes.emplace_back(std::move(s));
}

void Art::drawItemSprite(Rectangle shape, Rectangle region, Color color, float layer) {
    auto s = Shape{};

    s.type = ITEM_SPRITE;

    s.x = shape.x;
    s.y = shape.y;
    s.width = shape.width;
    s.height = shape.height;

    s.region = region;

    s.color = color;
    s.layer = layer;

    shapes.emplace_back(std::move(s));
}

void Art::drawEquipSprite(Rectangle shape, Rectangle region, Color color, float layer) {
    auto s = Shape{};

    s.type = EQUIP_SPRITE;

    s.x = shape.x;
    s.y = shape.y;
    s.width = shape.width;
    s.height = shape.height;

    s.region = region;

    s.color = color;
    s.layer = layer;

    shapes.emplace_back(std::move(s));
}

void Art::paint() {
    std::sort(shapes.begin(), shapes.end(), [&](auto& a, auto& b){
        return a.layer < b.layer;
    });

    for (auto& shape : shapes) {
        switch (shape.type) {
            case Shapes::RECT: {
                DrawRectanglePro(
                    shape.rect(),
                    Vector2{0, 0}, shape.rotation, shape.color
                );
                break;
            }
            case Shapes::LINE_RECT: {
                DrawRectangleLinesEx(shape.rect(), 1.0,shape.color);
                break;
            }
            case Shapes::ITEM_SPRITE:
            case Shapes::EQUIP_SPRITE: {
                Texture tex;

                if (shape.type == Shapes::ITEM_SPRITE) tex = Assets::I()->textures[TEX_ITEMS];
                else if (shape.type == Shapes::EQUIP_SPRITE) tex = Assets::I()->textures[TEX_EQUIPMENT];

                DrawTexturePro(tex, shape.region, shape.rect(), Vector2{0, 0}, shape.rotation, WHITE);
                break;
            }
        }
    }

    shapes.clear();
}

Color Art::strobeLight() {
    const float time = (float)GetTime() * 10.0f;
    const float alpha_scale = (((cosf(time) / 2.0f) + 0.5f) * 0.8f) + 0.2f;
    const uint8_t alpha = static_cast<uint8_t>(255 * alpha_scale);
    return Color{255, 255, 255, alpha};
}
