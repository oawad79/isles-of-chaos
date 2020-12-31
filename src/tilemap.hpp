#pragma once

#include <string>
#include <memory>
#include <raylib.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <entt.hpp>

extern "C" {
    #include <raymath.h>
}

#include <tuple>

#include "tinyxml2.hpp"
#include "utils.hpp"
#include "sprite.hpp"

using Tile = uint32_t;

struct Polygon {
    int id{-1};
    Vector2 position{0, 0};
    std::vector<Vector2> points{};
};

struct Line { Vector2 a{0, 0}, b{0, 0}; };

struct Object : Rectangle {
    int id{-1};
    std::string type{""};
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

struct Tilemap {
    Tileset tileset;
    std::vector<std::vector<Chunk*>> layers;
    std::vector<Polygon> geometry;
    std::vector<Object> objects;
    std::vector<Billboard> billboards;

    RenderTexture2D target;

    ~Tilemap();
};

uptr<Tilemap> LoadTilemap(entt::registry& reg, const std::string& path);
Tileset LoadTileset(tinyxml2::XMLElement* el);

void DrawTilemap(const uptr<Tilemap>& tilemap, SpriteRenderer& ren);
