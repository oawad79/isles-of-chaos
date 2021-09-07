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

}

void Art::paint() {

}
