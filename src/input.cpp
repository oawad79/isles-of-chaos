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

    if (IsGamepadAvailable(0)) {
        float x_axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        float y_axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        if (x_axis != 0.0f) result.x = x_axis;
        if (y_axis != 0.0f) result.y = y_axis;
    }

    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
      result.x = -1;
    }

    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
      result.x = 1;
    }

  return result;
}

auto Input::Climb() -> bool {
  return IsKeyDown(keymapping[InputMap::CLIMB]) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP); 
}

auto Input::Up() -> bool {
    return IsKeyDown(keymapping[InputMap::MOVE_UP])
        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)
        || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -0.5f;
}

auto Input::Down() -> bool {
    return IsKeyDown(keymapping[InputMap::MOVE_DOWN])
        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
}

auto Input::EnterDoor() -> bool {
    return IsKeyPressed(keymapping[InputMap::MOVE_UP])
        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)
		|| GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -0.5f;
}

auto Input::Jump() -> bool {
    return IsKeyPressed(keymapping[InputMap::JUMP])
    || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
}

auto Input::DodgeRoll() -> bool {
    return IsKeyPressed(keymapping[InputMap::DODGE_ROLL])
        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1);
}

auto Input::OpenInv() -> bool {
    return IsKeyPressed(keymapping[InputMap::OPEN_INVENTORY])
          || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP);
}

auto Input::Ascend() -> bool {
    return IsKeyDown(keymapping[InputMap::MOVE_UP])
        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)
        || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > 0.5f;
}

auto Input::Descend() -> bool {
    return IsKeyDown(keymapping[InputMap::MOVE_DOWN])
        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)
        || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > 0.5f;
}

auto Input::ShouldPause() -> bool {
    return IsKeyPressed(keymapping[InputMap::PAUSE])
        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT);
}

auto Input::Attack() -> bool {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON)
        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
}

auto Input::Interact() -> bool {
    return IsKeyPressed(keymapping[InputMap::INTERACT])
        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
}
