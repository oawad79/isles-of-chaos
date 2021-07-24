//
// Created by dusti on 7/1/2021.
//

#ifndef ISLESOFCHAOS_WINDOW_SIZING_HPP
#define ISLESOFCHAOS_WINDOW_SIZING_HPP

#include <raylib.h>
#include <raymath.h>
#include <iostream>
// We do this because we want to know what the window size is, when calling
// GetScreenWidth() it gives you the currently bound fbos size, so not really the window...
// We should move this into an event listener for resize

Vector2 GetWindowSize(bool set=false);

#endif//ISLESOFCHAOS_WINDOW_SIZING_HPP
