#pragma once

#include <optional>

constexpr auto BANNER_MAX_TIME { 4.0f };

struct BannerState {
  std::string text{""};
  float timeLeft{0.0f};
  Color color{WHITE};
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
};

