#include "gui_state.hpp"
#include <iostream>

void GuiState::setPadding(float newPadding) {
  padding = newPadding;
}

bool GuiState::doButton(Rectangle shape, const std::string& title, int fontSize, uint64_t flags) {
  const auto& defFont = GetFontDefault();

  float x = shape.x - padding;
  float y = shape.y - padding;

  if (flags & GUI_FLAG_CONTAINER_CENTER_X) x -= (shape.width + padding * 2) / 2.0f;
  if (flags & GUI_FLAG_CONTAINER_CENTER_Y) y -= (shape.height + padding * 2) / 2.0f;

  float fx = x + padding;
  float fy = y + padding;

  if (flags & GUI_FLAG_CENTER_X || flags & GUI_FLAG_CENTER_Y) {
    const auto strSize = MeasureTextEx(defFont, title.c_str(), fontSize, 1.0f);
    if (flags & GUI_FLAG_CENTER_X) fx = x + shape.width / 2 - strSize.x / 2;
    if (flags & GUI_FLAG_CENTER_Y) fy = y + shape.height / 2 - strSize.y / 2;
  }

  bool hot = CheckCollisionPointRec(MousePositionCanvasSpace(), Rectangle{x, y, shape.width + padding*2.0f, shape.height + padding*2.0f});

  DrawRectangle(x, y, shape.width + padding*2.f, shape.height + padding*2.f, BTN_BG);
  if (hot) DrawRectangleLines(x, y, shape.width + padding*2.f, shape.height + padding*2.f, BTN_FG);
  DrawTextEx(defFont, title.c_str(), Vector2{fx, fy}, fontSize, 1.0f, BTN_FG);

  return hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
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
