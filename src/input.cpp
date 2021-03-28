#include "input.hpp"

void DrawDebugInputOverlay() {

}

Vector2 Input::GetMovementVector() {
    auto result = Vector2{0, 0};

    if (IsKeyDown(keymapping[InputMap::MOVE_RIGHT]))
        result.x = 1;

    if (IsKeyDown(keymapping[InputMap::MOVE_LEFT]))
        result.x = -1;

    if (IsKeyDown(keymapping[InputMap::MOVE_DOWN]))
        result.y = 1;

    if (IsKeyDown(keymapping[InputMap::MOVE_UP]))
        result.y = -1;

    if (IsGamepadAvailable(GAMEPAD_PLAYER1)) {
        float x_axis = GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_LEFT_X);
        float y_axis = GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_LEFT_Y);
        if (x_axis != 0.0f) result.x = x_axis;
        if (y_axis != 0.0f) result.y = y_axis;
    }

    return result;
}

auto Input::Climb() -> bool {
    return IsKeyDown(keymapping[InputMap::CLIMB]);
}

auto Input::Up() -> bool {
    return IsKeyDown(keymapping[InputMap::MOVE_UP]);
}

auto Input::Down() -> bool {
    return IsKeyDown(keymapping[InputMap::MOVE_DOWN]);
}

auto Input::EnterDoor() -> bool {
    return IsKeyPressed(keymapping[InputMap::MOVE_UP]);
}

auto Input::Jump() -> bool {
    return IsKeyPressed(keymapping[InputMap::JUMP]);
}

auto Input::OpenInv() -> bool {
    return IsKeyPressed(keymapping[InputMap::OPEN_INVENTORY]);
}

auto Input::Ascend() -> bool {
    return IsKeyDown(keymapping[InputMap::MOVE_UP]);
}

auto Input::Descend() -> bool {
    return IsKeyDown(keymapping[InputMap::MOVE_DOWN]);
}

auto Input::ShouldPause() -> bool {
    return IsKeyPressed(keymapping[InputMap::PAUSE]);
}

auto Input::Attack() -> bool {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
