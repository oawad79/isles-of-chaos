#include "tilemap.hpp"
#include "water.hpp"

Tilemap::~Tilemap() {
    for (auto& layer : layers) {
        for (auto* chunk : layer) {
            delete chunk;
            chunk = nullptr;
        }
    }
}

Tileset LoadTileset(tinyxml2::XMLElement* el) {
    Tileset result{};

    using namespace tinyxml2;

    auto* image = el->FirstChildElement();

    int width{0}, height{0};
    el->QueryIntAttribute("tilewidth", &width);
    el->QueryIntAttribute("tileheight", &height);

    const auto path = std::filesystem::path(image->Attribute("source"));
    const auto texturePath = "resources/textures/" + path.filename().string();

    result.texture = LoadTexture(texturePath.c_str());
    result.tilewidth = width;
    result.tileheight = height;

    return result;
}

Tilemap* LoadTilemap(const std::string& path) { 
    if (std::filesystem::exists(path) == false) {
        std::cout << "ERROR::Tilemap::LoadTilemap:: map does not exist: "
                  << path
                  << std::endl;
        return nullptr;
    }

    auto map = new Tilemap();

    map->path = path;
    map->name = std::filesystem::path(path).stem().string();

    map->target = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
    map->foregroundTarget = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);

    using namespace tinyxml2;

    XMLDocument document;
    document.LoadFile(path.c_str());

    auto* root = document.FirstChild();
    auto* mapd = root->NextSiblingElement();
    auto* tilesetd = mapd->FirstChildElement("tileset");

    const char* mapColor = mapd->Attribute("backgroundcolor");
    if (mapColor) {
        auto color = std::string{mapColor};
        map->backgroundColor = ColorFromHexString(color);
        std::cout << color << std::endl;
    }

    if (!tilesetd) {
        std::cout << "Failed to load a tileset, none was embedded in the map: " << path << std::endl;
        return map;
    }

    map->tileset = LoadTileset(tilesetd);

    auto* layer = mapd->FirstChildElement("layer");
    while (layer) {
        auto theLayer = Layer();
        const char* ctint = layer->Attribute("tintcolor");
        if (ctint) {
            auto tint = std::string{ctint};
            theLayer.tint = ColorFromHexString(tint);
        }
        map->layers.push_back(theLayer);

        std::string name{layer->Name()};

        auto* datad = layer->FirstChildElement("data");

        if (name == "layer" && datad) {
            auto* chunkd = datad->FirstChildElement("chunk");

            while (chunkd) {
                auto* chunk = new Chunk();

                int cwidth{0}, cheight{0};
                int coffsetx{0}, coffsety{0};
                chunkd->QueryIntAttribute("width", &cwidth);
                chunkd->QueryIntAttribute("height", &cheight);
                chunkd->QueryAttribute("x", &coffsetx);
                chunkd->QueryAttribute("y", &coffsety); 

                chunk->tiles.reserve(sizeof(Tile)*cwidth*cheight);
                chunk->offset = Vector2{(float)coffsetx, (float)coffsety};
                chunk->size = Vector2{(float)cwidth, (float)cheight};

                const auto text = std::string{chunkd->GetText()};
                std::stringstream ss(text);

                int index = 0;
                for (Tile i; ss >> i;) {
                        chunk->tiles.push_back(i);
                    if (ss.peek() == ',') ss.ignore();
                }

                map->layers[map->layers.size()-1].push_back(chunk);
                chunkd = chunkd->NextSiblingElement();
            }
        }

        if (name == "objectgroup") {
            auto* object = layer->FirstChildElement("object");
            while (object) {

                Vector2 polypos {0, 0};
                object->QueryFloatAttribute("x", &polypos.x);
                object->QueryFloatAttribute("y", &polypos.y);

                int id = 0;
                object->QueryIntAttribute("id", &id);

                auto* polygond = object->FirstChildElement("polygon");
                if (polygond) {
                    std::string points_txt { polygond->Attribute("points") };
                    TraceLog(LOG_INFO, "%s: %s", "Loading polygon", points_txt.c_str());

                    Polygon polygon;
                    polygon.type = SolidType::Polygon;
                    polygon.x = polypos.x;
                    polygon.y = polypos.y;
                    polygon.id = id;

                    std::stringstream ss(points_txt);

                    for (std::string s; ss >> s;) {
                        auto it = s.begin();
                        auto beg = it;

                        float x = 0;
                        float y = 0;

                        while (it != end(s)) {
                            if (*it == ',') {
                                x = std::stof(std::string(beg, it));
                                beg = it + 1;
                                it++;
                            }
                            it++;
                        }

                        y = std::stof(std::string(beg, it));

                        polygon.points.push_back(Vector2{x, y});
                    }

                    map->geometry.push_back(polygon);
                } else {
                    Vector2 size{0, 0};
                    object->QueryFloatAttribute("width", &size.x);
                    object->QueryFloatAttribute("height", &size.y);

                    const char* type = object->Attribute("type");

                    if (type) {
                        const auto stype = std::string{type};
                        if (stype == "billboard") {
                          Billboard billboard;

                          billboard.texture = map->tileset.texture;
                          billboard.T = Type::SPRITE;
                          billboard.position.x = polypos.x;
                          billboard.position.y = polypos.y;

                          const auto *props = object->FirstChildElement();
                          auto *prop = props->FirstChildElement();
                          while (prop) {
                            const auto *name = prop->Attribute("name");

                            if (strcmp(name, "region") == 0) {
                              std::stringstream ss(prop->Attribute("value"));
                              ss >> billboard.region.x;
                              ss >> billboard.region.y;
                              ss >> billboard.region.width;
                              ss >> billboard.region.height;
                            }

                            prop = prop->NextSiblingElement();
                          }

                          map->billboards.push_back(billboard);
                        } else if (stype ==  "Water") {
                          map->features.push_back(MakeFeature(FeatureType::Water, polypos, size));
                        } else if (stype ==  "Barrier") {
                          map->features.push_back(MakeFeature(FeatureType::Barrier, polypos, size));
                        } else if (stype == "Banner") {
                            auto feat = MakeFeature(FeatureType::Banner, polypos, size);

                            const auto* props = object->FirstChildElement();
                            if (!props) {
                                std::cout << "Error: Banner is missing props" << std::endl;
                            } else {
                                auto* prop = props->FirstChildElement();
                                while (prop) {
                                    const auto name = std::string{prop->Attribute("name")};
                                    if (name == "banner") {
                                        feat.target = std::string{prop->Attribute("value")};
                                    }
                                    prop = prop->NextSiblingElement();
                                }
                            }

                            map->features.push_back(std::move(feat));
                        } else if (stype == "Checkpoint") {
                            map->features.push_back(MakeFeature(FeatureType::Checkpoint, polypos, size));
                        } else if (stype == "Kill") {
                            map->features.push_back(MakeFeature(FeatureType::Kill, polypos, size));
                        } else if (stype == "Room") {
                            map->rooms.push_back(MakeFeature(FeatureType::Room, polypos, size));
                        } else if (stype == "Port" || stype == "Door") {
                            auto feat = MakeFeature(stype == "Port" ? FeatureType::Port : FeatureType::Door, polypos, size);

                            const auto* props = object->FirstChildElement();
                            if (!props) {
                                std::cout << "Error: Port/Door is missing props" << std::endl;
                            } else {
                                auto* prop = props->FirstChildElement();
                                while (prop) {
                                    const auto name = std::string{prop->Attribute("name")};
                                    if (name == "target") {
                                        feat.target = std::string{prop->Attribute("value")};
                                    } else if (name == "id") {
                                        feat.id = std::string{prop->Attribute("value")};
                                    } else {
                                        std::cout << "WARNING:: Port has unsupported property: "
                                                << name
                                                << std::endl;
                                    }
                                    prop = prop->NextSiblingElement();
                                }
                            }

                            map->features.push_back(std::move(feat));
                        } else if (stype == "Ladder") {
                            map->features.push_back(MakeFeature(FeatureType::Ladder, polypos, size));
                        } else {
                            SpawnLocation loc;
                            loc.type = GetEntType(stype);
                            loc.x = polypos.x;
                            loc.y = polypos.y;
                            loc.width = size.x;
                            loc.height = size.y;

                            const auto* props = object->FirstChildElement();
                            if (props) {
                                auto* prop = props->FirstChildElement();
                                while (prop) {
                                    const auto* name = prop->Attribute("name");
                                    if (strcmp(name, "id") == 0) {
                                        loc.id = std::string{prop->Attribute("value")};
                                        std::cout << "ID: " << loc.id << std::endl;
                                    } else {
                                        loc.props[std::string{name}] = std::string{prop->Attribute("value")};
                                    }

                                    prop = props->NextSiblingElement();
                                }
                            }

                            map->objects.push_back(std::move(loc));
                        }
                    } else {
                        Polygon obj;
                        obj.id = id;
                        obj.type = SolidType::Rectangle;
                        obj.x = polypos.x;
                        obj.y = polypos.y;
                        obj.width = size.x;
                        obj.height = size.y;
                        map->geometry.push_back(std::move(obj));
                    }
                }

                object = object->NextSiblingElement();
            }
        }

        layer = layer->NextSiblingElement();
    }

    std::cout << "TILEMAP::Loaded:: " << path << std::endl;

    return map;
}

void DrawTilemapToTarget(const uptr<Tilemap>& tilemap, const Camera2D camera, SpriteRenderer& ren) {
    DrawTilemapToTarget(tilemap.get(), camera, ren);
}

void UpdateTilemap(Tilemap* tilemap) {
  for (auto& feat : tilemap->features)
    if (feat.type == FeatureType::Water)
      UpdateWater(feat);
}

void DrawTilemap(const uptr<Tilemap>& tilemap) {
    DrawTilemap(tilemap.get());
}

void DrawTilemapToTarget(const Tilemap* tilemap, const Camera2D camera, SpriteRenderer& ren) {
    const auto [ox, oy] = tilemap->position;
    BeginTextureMode(tilemap->target);
        BeginMode2D(camera);
        ClearBackground({0,0,0,0});
        const auto* tileset = &tilemap->tileset;

//        bool roomActive = false;
//        for (auto& room : tilemap->rooms) {
//            if (room.active) {
//                BeginScissorMode((int)room.x, (int)room.y, (int)room.width, (int)room.height);
//                roomActive = true;
//            }
//        }

        for (const auto& layer : tilemap->layers) {
            const auto tint = layer.tint;
            for (const auto& chunk : layer) {
                for (int y = 0; y < chunk->size.y; y++) {
                    for (int x = 0; x < chunk->size.x; x++) {
                        int index = x + y * chunk->size.x;

                        Tile tile = chunk->tiles[index];

                        if (tile == 0) continue;
                        --tile;

                        float rx = (tile%(tileset->texture.width/tileset->tilewidth)) * 1.0f;
                        float ry = (tile/(tileset->texture.width/tileset->tilewidth)) * 1.0f;

                        Rectangle region{
                            rx*(float)tileset->tilewidth,
                            ry*(float)tileset->tileheight,
                            (float)tileset->tilewidth,
                            (float)tileset->tileheight };

                        const float cox = (float)tileset->tilewidth * (chunk->offset.x);
                        const float coy = (float)tileset->tileheight * (chunk->offset.y);

                        DrawTextureRec(
                            tileset->texture,
                            region,
                            Vector2{
                                floor(cox + (float)x*(float)tileset->tilewidth + ox),
                                floor(coy + (float)y*(float)tileset->tileheight + oy)
                            },
                            tint);
                    }
                }
            }
        }

        for (auto& feat : tilemap->features) {
          if (feat.type == FeatureType::Barrier) {
            const auto region = Rectangle{40, 0, 8, 8};
            auto tw = (int)floor(feat.width / tileset->tilewidth);
            auto th = (int)floor(feat.height / tileset->tileheight);

            for (int y = 0; y < th; y++) {
              for (int x = 0; x < tw; x++) {
                DrawTextureRec(
                    tileset->texture,
                    region,
                    Vector2{
                      floor(feat.x + (float)x*(float)tileset->tilewidth + ox),
                      floor(feat.y + (float)y*(float)tileset->tileheight + oy)
                    },
                    WHITE);
              }
            }
          }
        }

        EndMode2D();
    EndTextureMode();

    BeginTextureMode(tilemap->foregroundTarget);
      ClearBackground({0,0,0,0});
      BeginMode2D(camera);
        for (const auto& feat : tilemap->features)
          if (feat.type == FeatureType::Water)
            DrawWater(feat);
      EndMode2D();
    EndTextureMode();
}

void DrawTilemap(const Tilemap* tilemap) {
    DrawTexturePro(
      tilemap->target.texture,
      {0,0,CANVAS_WIDTH,-CANVAS_HEIGHT},
      {0,0,CANVAS_WIDTH,CANVAS_HEIGHT},
      Vector2Zero(),
      0.0f,
      {255, 255, 255, static_cast<unsigned char>(255.0f * tilemap->alpha)});
}

void DrawTilemapForeground(const Tilemap* tilemap) {
  DrawTexturePro(
    tilemap->foregroundTarget.texture,
    {0,0,CANVAS_WIDTH,-CANVAS_HEIGHT},
    {0,0,CANVAS_WIDTH,CANVAS_HEIGHT},
    Vector2Zero(),
    0.0f,
    {255, 255, 255, static_cast<unsigned char>(255.0f * tilemap->alpha)});
}

std::optional<Feature> GetPortWithTarget(
    const Tilemap* tilemap,
    const std::string& target,
    const std::string& id) {

    for (const auto& feat : tilemap->features) {
        if (feat.type == FeatureType::Port) {
            if (feat.target == target && feat.id == id)
                return std::optional{feat};
        }
    }

    return std::nullopt;
}

std::optional<Feature> GetDoorWithTarget(
    const Tilemap* tilemap,
    const std::string& target,
    const std::string& id) {

    for (const auto& feat : tilemap->features) {
        if (feat.type == FeatureType::Door) {
            if (feat.target == target && feat.id == id)
                return std::optional{feat};
        }
    }

    return std::nullopt;
}

void UpdateTilemapGeometryPositions(Tilemap* tilemap) {
    const auto [x, y] = tilemap->position;
    for (auto& feat : tilemap->features) {
        feat.offset.x = x;
        feat.offset.y = y;
    }
    for (auto& geom : tilemap->geometry) {
        geom.offset.x = x;
        geom.offset.y = y;
    }
    for (auto& spawn : tilemap->objects) {
        spawn.offset.x = x;
        spawn.offset.y = y;
    }
}

void SetPosition(Tilemap* tilemap, Vector2 newPos) {
    tilemap->position = newPos;
    UpdateTilemapGeometryPositions(tilemap);
}
