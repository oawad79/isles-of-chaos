#ifndef SKYVAULT_LEVEL_HPP
#define SKYVAULT_LEVEL_HPP

#include <memory>
#include <vector>
#include <optional>

#include "utils.hpp"
#include "entt.hpp"
#include "tilemap.hpp"

/*
** Encapsulates the tilemaps, stitches them together
*/
struct Level {
    std::vector<Tilemap*> tilemaps;
    std::string currentTilemap{""};
};

Tilemap* GetTilemap(const uptr<Level>& level);
Tilemap* GetTilemap(const uptr<Level>& level, const std::string& byName);
bool HasTilemap(const uptr<Level>& level, const std::string& path);

uptr<Level> LoadLevel(const std::string& path);
void UpdateLevel(Level& level, entt::registry& reg);

#endif // SKYVAULT_LEVEL_HPP
