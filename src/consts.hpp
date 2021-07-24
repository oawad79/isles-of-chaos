#ifndef SKYVAULT_CONSTS
#define SKYVAULT_CONSTS

#include <string_view>

void LoadSettingsFile(const std::string_view& path);

constexpr auto START_GAME_LEVEL { "StartIsland1" };

constexpr auto CANVAS_WIDTH { 256.0f };
constexpr auto CANVAS_HEIGHT { 144.0f };

constexpr auto GUI_CANVAS_WIDTH { 256.0f };
constexpr auto GUI_CANVAS_HEIGHT { 144.0f };

constexpr auto CAMERA_ZOOM {12.0f};

#endif // SKYVAULT_CONSTS
