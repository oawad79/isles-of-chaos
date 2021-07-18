#pragma once

#include <optional>
#include <string>
#include <stdint.h>

#include "raylib.h"

#include "assets.hpp"
#include "consts.hpp"
#include "item.hpp"
#include "window_sizing.hpp"

constexpr auto BANNER_MAX_TIME { 4.0f };

constexpr Color BTN_FG { 200, 200, 180, 255 };
constexpr Color BTN_BG { 20, 20, 18, 255 };

enum GuiFlag : uint64_t {
  GUI_FLAG_NONE               = 1lu << 0lu,
  GUI_FLAG_CENTER_X           = 1lu << 1lu,
  GUI_FLAG_CENTER_Y           = 1lu << 2lu,
  GUI_FLAG_CONTAINER_CENTER_X = 1lu << 3lu,
  GUI_FLAG_CONTAINER_CENTER_Y = 1lu << 4lu,
  GUI_FLAG_RIGHT              = 1lu << 5lu,
};

struct Button {
  Rectangle position{0};
  std::string title;
};

enum BannerFadeState {
  BFS_NONE,
  BFS_IN,
  BFS_HOLD,
  BFS_OUT,
};

struct BannerState {
  std::string text{""};
  BannerFadeState state{BFS_NONE};
  float timeLeft{0.0f};
  float alpha{0.0f};
};

struct GuiState {
    bool playerInvOpen{false}; 

    Rectangle healthRegion{64, 16, 32, 16};

    int healthFrame{0};
    float healthFrameTimer{0.0f};
    float speed{6.0f};
    float frameScaler{0.0f}; // Goes from -2.0 to 2.0
    float frameTarget{0.0f};

    BannerState banner;

    std::optional<Item> dragItem;

    void setPadding(float newPadding);
    bool doButton(Rectangle shape, const std::string& title, int fontSize=10, uint64_t flags=GUI_FLAG_NONE);

private:
    float padding{2.0f};
};

Vector2 GetMouseGuiPosition();
Vector2 MousePositionCanvasSpace();
Vector2 MouseGuiCanvasPosition();

