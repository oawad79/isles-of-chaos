#include "tilemap.hpp"

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
    const auto filename = path.filename();

    char fullpath[512];
    sprintf(fullpath, "resources/textures/%s", filename.c_str());

    result.texture = LoadTexture(fullpath);
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
    map->name = std::filesystem::path(path).stem();
    map->target = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);

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
                    chunk->tiles[index++] = i;
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

                            const auto* props = object->FirstChildElement();
                            auto* prop = props->FirstChildElement();
                            while (prop) {
                                const auto* name = prop->Attribute("name");

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
                        } else if (stype == "Checkpoint") {
                            Feature feat;
                            feat.type = FeatureType::Checkpoint;
                            feat.x = polypos.x;
                            feat.y = polypos.y;
                            feat.width = size.x;
                            feat.height = size.y;
                            map->features.push_back(std::move(feat));
                        } else if (stype == "Kill") {
                            Feature feat;
                            feat.type = FeatureType::Kill;
                            feat.x = polypos.x;
                            feat.y = polypos.y;
                            feat.width = size.x;
                            feat.height = size.y;
                            map->features.push_back(std::move(feat));
                        } else if (stype == "Port") {
                            Feature feat;
                            feat.type = FeatureType::Port;
                            feat.x = polypos.x;
                            feat.y = polypos.y;
                            feat.width = size.x;
                            feat.height = size.y;

                            const auto* props = object->FirstChildElement();
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

                            map->features.push_back(std::move(feat));
                        } else if (stype == "Ladder") {
                            Feature feat;
                            feat.type = FeatureType::Ladder;
                            feat.x = polypos.x;
                            feat.y = polypos.y;
                            feat.width = size.x;
                            feat.height = size.y;
                            map->features.push_back(std::move(feat));
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

void DrawTilemap(const uptr<Tilemap>& tilemap) {
    DrawTilemap(tilemap.get());
}

void DrawTilemapToTarget(const Tilemap* tilemap, const Camera2D camera, SpriteRenderer& ren) {
    const auto [ox, oy] = tilemap->position;
    BeginTextureMode(tilemap->target);
        BeginMode2D(camera);
        ClearBackground({0,0,0,0});
        const auto* tileset = &tilemap->tileset;

        // for (const auto& bill : tilemap->billboards) {
        //     DrawSprite(
        //         ren,
        //         (Sprite)bill,
        //         Body{
        //             bill.position.x + ox,
        //             bill.position.y + oy,
        //             bill.region.width,
        //             bill.region.height,
        //         });
        // }

        for (const auto& layer : tilemap->layers) {
            const auto tint = layer.tint;
            for (const auto& chunk : layer) {
                for (int y = 0; y < chunk->size.y; y++) {
                    for (int x = 0; x < chunk->size.x; x++) {
                        int index = x + y * chunk->size.x;

                        Tile tile = chunk->tiles[index];

                        if (tile == 0) continue;
                        --tile;

                        float rx = tile%(tileset->texture.width/tileset->tilewidth);
                        float ry = tile/(tileset->texture.width/tileset->tilewidth);

                        Rectangle region{
                            rx*tileset->tilewidth,
                            ry*tileset->tileheight,
                            (float)tileset->tilewidth,
                            (float)tileset->tileheight };

                        const float cox = tileset->tilewidth * (chunk->offset.x);
                        const float coy = tileset->tileheight * (chunk->offset.y);

                        DrawTextureRec(
                            tileset->texture,
                            region,
                            Vector2{
                                floor(cox + (float)x*tileset->tilewidth + ox),
                                floor(coy + (float)y*tileset->tileheight + oy)},
                            tint);
                    }
                }
            }
        }
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
        {255, 255, 255,
         static_cast<unsigned char>(255.0f * tilemap->alpha)});
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
