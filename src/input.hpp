#pragma once

#include <memory>
#include <mutex>
#include <algorithm>
#include <raylib.h>
#include <map>
#include <string>
#include <iostream>

#include "utils.hpp"

#define FOREACH_INPUT_TYPE(I)\
    I(MOVE_LEFT)\
    I(MOVE_RIGHT)\
    I(MOVE_UP)\
    I(MOVE_DOWN)\
    I(ATTACK)\
    I(ACTION)\
    I(CLIMB)\
    I(PAUSE)\
    I(NUM_INPUT_TYPES)

enum class InputMap {FOREACH_INPUT_TYPE(GENERATE_ENUM)};
const std::string InputMapStrs[] {FOREACH_INPUT_TYPE(GENERATE_STRING)};

struct Input {
    static auto* I() {
        std::call_once(Input::once, [](){
            it.reset(new Input());
        });
        return it.get();
    }

    inline Input() {}
    inline ~Input() {}

    Vector2 GetMovementVector();

    auto ShouldPause() -> bool;
    auto Climb() -> bool;
    auto Up() -> bool;
    auto Down() -> bool;

private:
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    static uptr<Input> it;
    static std::once_flag once;

    std::map<InputMap, KeyboardKey> keymapping {
        {InputMap::MOVE_LEFT, KEY_LEFT},
        {InputMap::MOVE_RIGHT, KEY_RIGHT},
        {InputMap::MOVE_UP, KEY_UP},
        {InputMap::MOVE_DOWN, KEY_DOWN},
        {InputMap::ATTACK, KEY_M},
        {InputMap::ACTION, KEY_PERIOD},
        {InputMap::CLIMB, KEY_W},
        {InputMap::PAUSE, KEY_ESCAPE},
    };
};
