#ifndef SKYVAULT_SCENE_HPP
#define SKYVAULT_SCENE_HPP

#include "utils.hpp"

struct Game;
class SceneLayer {
public:
    virtual void load(const uptr<Game>& game) {}
    virtual void update(const uptr<Game>& game){}
    virtual void render(const uptr<Game>& game){}
    virtual void destroy(const uptr<Game>& game){}
};

#endif // SKYVAULT_SCENE_HPP
