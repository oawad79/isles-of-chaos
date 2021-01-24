#pragma once

#include <string>
#include <memory>
#include <raylib.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <optional>
#include <entt.hpp>

extern "C" {
    #include <raymath.h>
}

#include <tuple>

#include "tinyxml2.hpp"
#include "utils.hpp"
#include "sprite.hpp"
#include "consts.hpp"
#include "enttypes.hpp"

using Tile = uint32_t;

enum class SolidType {
    Rectangle,
    Polygon
};

enum class FeatureType {
    None,
    Ladder,
    Port,
    Door,
    Checkpoint,
    Kill,
};

struct Polygon : Rectangle {
    Vector2 offset{0, 0};

    int id{-1};
    SolidType type{SolidType::Rectangle};
    std::vector<Vector2> points{};

    inline auto bounds() const { return Rectangle{
        x + offset.x,
        y + offset.y,
        width,
        height
    }; }
};

struct Line { Vector2 a{0, 0}, b{0, 0}; };

struct SpawnLocation : Rectangle {
    Vector2 offset{0, 0};

    EntType type{EntType::None};
    std::string id{""}; // Used for items

    inline auto bounds() const { return Rectangle{
        x + offset.x,
        y + offset.y,
        width,
        height
    }; }
};

struct Feature : Rectangle {
    Vector2 offset{0, 0};

    FeatureType type{FeatureType::None};
    std::string target{""};
    std::string id{""};

    inline auto bounds() const { return Rectangle{
        x + offset.x,
        y + offset.y,
        width,
        height
    }; }
};

struct Chunk {
    Vector2 offset;
    Vector2 size;

    std::vector<Tile> tiles;
};

struct Tileset {
    Texture2D texture;
    int tilewidth{8};
    int tileheight{8};
};

struct Billboard : Sprite {
    Vector2 position;
};

struct Layer : std::vector<Chunk*> {
    Color tint{WHITE};
};

struct Tilemap {
    std::string path{""};
    std::string name{""};

    Color backgroundColor{0, 0, 0, 0};
    Vector2 position{0, 0};

    Tileset tileset;
    std::vector<Layer> layers;
    std::vector<Polygon> geometry;
    std::vector<SpawnLocation> objects;
    std::vector<Feature> features; // Ladders / Doors / Ports
    std::vector<Billboard> billboards;

    RenderTexture2D target;
    float alpha {1.0f};

    bool positionUpdated{false};

    bool spawnedEntities{false};
    int lastSpawnTime{0};

    ~Tilemap();
};

std::optional<Feature> GetPortWithTarget(
    const Tilemap* tilemap,
    const std::string& target,
    const std::string& id);

void UpdateTilemapGeometryPositions(Tilemap* tilemap);
void SetPosition(Tilemap* tilemap, Vector2 newPos);

Tilemap* LoadTilemap(const std::string& path);
Tileset LoadTileset(tinyxml2::XMLElement* el);

void DrawTilemapToTarget(const uptr<Tilemap>& tilemap, const Camera2D camera, SpriteRenderer& ren);
void DrawTilemap(const uptr<Tilemap>& tilemap);

void DrawTilemapToTarget(const Tilemap* tilemap, const Camera2D camera, SpriteRenderer& ren);
void DrawTilemap(const Tilemap* tilemap);
