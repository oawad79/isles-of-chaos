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

uptr<Tilemap> LoadTilemap(entt::registry& reg, const std::string& path) {
    auto map = std::make_unique<Tilemap>();

    map->target = LoadRenderTexture(1280, 720);

    using namespace tinyxml2;

    XMLDocument document;
    document.LoadFile(path.c_str());

    auto* root = document.FirstChild();
    auto* mapd = root->NextSiblingElement();
    auto* tilesetd = mapd->FirstChildElement("tileset");

    if (!tilesetd) {
        std::cout << "Failed to load a tileset, none was embedded in the map: " << path << std::endl;
        return map;
    }

    map->tileset = LoadTileset(tilesetd);

    auto* layer = mapd->FirstChildElement("layer");
    while (layer) {
        map->layers.push_back(std::vector<Chunk*>());

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
                    polygon.position = polypos;
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
                        if (strcmp(type, "billboard") == 0) {
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
                        } else if (strcmp(type, "ladder") == 0) {
                            Object obj;
                            obj.id = id;
                            obj.type = std::string{type};
                            obj.x = polypos.x;
                            obj.y = polypos.y;
                            obj.width = size.x;
                            obj.height = size.y;
                            map->objects.push_back(std::move(obj));
                        }
                    } else {
                        Polygon obj;
                        obj.id = id;
                        obj.type = SolidType::RECTANGLE;
                        obj.position.x = polypos.x;
                        obj.position.y = polypos.y;
                        obj.size.x = size.x;
                        obj.size.y = size.y;
                        map->geometry.push_back(std::move(obj));
                    }
                }

                object = object->NextSiblingElement();
            }
        }

        layer = layer->NextSiblingElement();
    }

    return map;
}

void DrawTilemap(const uptr<Tilemap>& tilemap, SpriteRenderer& ren) {
    const auto* tileset = &tilemap->tileset;

    for (const auto& bill : tilemap->billboards) {
        DrawSprite(
            ren,
            (Sprite)bill,
            Body{
                bill.position.x,
                bill.position.y,
                bill.region.width,
                bill.region.height,
            });
    }

    for (const auto& layer : tilemap->layers) {
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

                    const float ox = tileset->tilewidth * (chunk->offset.x);
                    const float oy = tileset->tileheight * (chunk->offset.y);

                    DrawTextureRec(
                        tileset->texture,
                        region,
                        Vector2{
                            floor(ox + (float)x*tileset->tilewidth),
                            floor(oy + (float)y*tileset->tileheight)},
                        WHITE);
                }
            }
        }
    }
}
