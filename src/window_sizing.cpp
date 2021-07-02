#include "window_sizing.hpp"

Vector2 GetWindowSize(bool set) {
  static Vector2 _windowSize{Vector2Zero()};
  if (set) _windowSize = Vector2{ static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) };
  return Vector2{ _windowSize.x, _windowSize.y };
}

