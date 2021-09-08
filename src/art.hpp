#ifndef SKYVAULT_ART_HPP
#define SKYVAULT_ART_HPP

#include <memory>
#include <mutex>
#include <algorithm>
#include <vector>
#include <cmath>
#include <raylib.h>

#include "utils.hpp"
#include "assets.hpp"

#define FCOLOR(r, g, b, a)                                                 \
  Color {                                                                  \
    (unsigned char)((r)*255), (unsigned char)((g)*255),                    \
    (unsigned char)((b)*255), (unsigned char)((a)*255)                     \
  }

#define VEC3_TO_COLOR(v) FCOLOR(v.x, v.y, v.z, 1.0f)
#define VEC4_TO_COLOR(v) FCOLOR((v).x, (v).y, (v).z, (v).w)

#define COLOR_TO_VEC4(c) Vector4{c.r/255.0f, c.g/255.0f, c.b/255.0f, c.a/255.0f};

enum Shapes {
  RECT,
  LINE_RECT,

  ITEM_SPRITE,
  EQUIP_SPRITE,
};

struct Shape {
  Shapes type{Shapes::RECT};

  float x, y;
  float width, height;
  float radius;
  float rotation;
  float layer;

  Rectangle region;
  Color color;

  inline auto rect() -> Rectangle {
    return Rectangle{x, y, width, height};
  }
};

struct Art {
  static auto* I() {
      std::call_once(Art::once, [](){
          it.reset(new Art());
      });
      return it.get();
  }

  void drawRect(float x, float y, float w, float h, Color color, float layer=0.0);
  void drawRectL(float x, float y, float w, float h, Color color, float layer=0.0);

  void drawItemSprite(Rectangle shape, Rectangle region, Color color, float layer=0.0);
  void drawEquipSprite(Rectangle shape, Rectangle region, Color color, float layer=0.0);

  void paint();

  inline Art() {}
  inline ~Art() {}

  Color strobeLight();
private:
  static uptr<Art> it;
  static std::once_flag once;

  std::vector<Shape> shapes;
};

#endif//SKYVAULT_ART_HPP
