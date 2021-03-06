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
    if (!level.get()) return false;
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
    if (!level.get()) return nullptr;
    for (auto& tm : level->tilemaps)
        if (tm->name == byName) return tm;
    return nullptr;
}

int GetTilemapId(const uptr<Level>& level) {
    int id = 0;
    for (auto& tm : level->tilemaps) {
        if (tm->name == level->currentTilemap)
            return id;
        id++;
    }
    return id;
}

uptr<Level> LoadLevel(const std::string& path) {
    auto level = std::make_unique<Level>();
    loadTilemapRecr(level, path);
    level->currentTilemap = std::filesystem::path(path).stem().string();
    return level;
}

void UpdateLevel(Level& level, entt::registry& reg) {

}
