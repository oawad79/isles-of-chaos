#pragma once

#include <raylib.h>
#include <raymath.h>

struct Body : Rectangle {
    inline Vector2 center() const { return Vector2{x+width/2, y+height/2}; }
    inline float dist(const Body& body) const {
        return Vector2Distance(center(), body.center());
    }
};
