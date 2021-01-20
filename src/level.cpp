#include "level.hpp"

void loadTilemapRecr(const uptr<Level>& level, const std::string& path) {
    auto tm = LoadTilemap(path);
    level->tilemaps.push_back(tm);
    for (const auto& feat : tm->features) {
        const std::string path =
            "resources/maps/" + feat.target + ".tmx";
        if (feat.type == FeatureType::Port && !HasTilemap(level, feat.target)) {
            loadTilemapRecr(level, path);
        }
    }
}

bool HasTilemap(const uptr<Level>& level, const std::string& name) {
    for (auto& tm : level->tilemaps) {
        if (tm->name == name)
            return true;
    }
    return false;
}

Tilemap* GetTilemap(const uptr<Level>& level) {
    return GetTilemap(level, level->currentTilemap);
}

Tilemap* GetTilemap(const uptr<Level>& level, const std::string& byName) {
    for (auto& tm : level->tilemaps)
        if (tm->name == byName) return tm;
    return nullptr;
}

uptr<Level> LoadLevel(const std::string& path) {
    auto level = std::make_unique<Level>();
    loadTilemapRecr(level, path);
    level->currentTilemap = std::filesystem::path(path).stem();
    return level;
}

void UpdateLevel(Level& level, entt::registry& reg) {

}
