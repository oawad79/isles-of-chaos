#include "gui_state.hpp"
#include <iostream>

void GuiState::setPadding(float newPadding) {
  padding = newPadding;
}

bool GuiState::doButton(Rectangle shape, const std::string& title, int fontSize, uint64_t flags) {
  const auto& defFont = GetFontDefault();

  const auto mpos = MousePositionCanvasSpace();
  bool hot = CheckCollisionPointRec(mpos, Rectangle{shape.x, shape.y, shape.width, shape.height});

  float x = shape.x - padding;
  float y = shape.y - padding;

  float fx = x;
  float fy = y;

  if (flags & GUI_FLAG_CENTER_X || flags & GUI_FLAG_CENTER_Y) {
    const auto strSize = MeasureTextEx(defFont, title.c_str(), fontSize, 1.0f);
    if (flags & GUI_FLAG_CENTER_X) {
      fx = shape.x + shape.width / 2 - strSize.x / 2;
    }
    if (flags & GUI_FLAG_CENTER_Y) {
      fy = shape.y + shape.height / 2 - strSize.y / 2;
    }
  }

  DrawRectangle(x, y, shape.width + padding*2.f, shape.height + padding*2.f, BTN_BG);

  if (hot) {
    DrawRectangleLines(x, y, shape.width + padding*2.f, shape.height + padding*2.f, BTN_FG);
  }

  DrawTextEx(defFont, title.c_str(), Vector2{fx, fy}, fontSize, 1.0f, BTN_FG);

  if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    return true;
  }

  return false;
}

Vector2 MousePositionCanvasSpace() {
  // TODO(Dustin): We need to handle letter boxing

  auto [mx, my] = GetMousePosition();
  auto [ww, wh] = GetWindowSize();

  const float scaleX = GUI_CANVAS_WIDTH / (float)ww;
  const float scaleY =  GUI_CANVAS_HEIGHT / (float)wh;

  mx *= scaleX;
  my *= scaleY;

  return Vector2{
    (float)mx,
    (float)my,
  };
}

Vector2 MouseGuiCanvasPosition() {
  const float mx = GetMouseX();
  const float my = GetMouseY();

  const float aspect = (float)GUI_CANVAS_HEIGHT / (float)GUI_CANVAS_WIDTH;
  const float width = GetScreenWidth();
  const float height = GetScreenHeight();

  const float ws = (float)GUI_CANVAS_WIDTH / (float)width;
  const float hs = (float)GUI_CANVAS_HEIGHT / (float)height;

  return {mx*ws, my*hs};
}
