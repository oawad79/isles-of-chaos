#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <algorithm>
#include <cassert>
#include <map>
#include <vector>
#include <raylib.h>
#include <string>
#include <sstream>
#include <filesystem>
#include <chrono>

#include "tinyxml2.hpp"
#include "utils.hpp"
#include "item.hpp"
#include "loot.hpp"

enum Textures {
    TEX_NONE,
    TEX_BG,
    TEX_GUI,
    TEX_EQUIPMENT,
    TEX_ENTITIES,
    TEX_PLAYER,
    TEX_ITEMS,
    TEX_OVERWORLD,
    TEX_PARTICLES,
    TEX_NUM_TEXTURES
};

enum Fonts {
    FONT_NONE,
    FONT_SMALL,
    FONT_FANCY,
    FONT_NUM_FONTS,
};

enum Shaders {
    SPRITE_SHADER,
    WATER_SHADER,
    SHADER_NUM_SHADERS,
};

constexpr int SHADER_RELOAD_SLOT = 32 - 1; // We get 32 from MAX_SHADER_LOCATIONS (config.h in raylib)

struct ShaderAssetInfo {
    std::filesystem::file_time_type vs_write_time;
    std::filesystem::file_time_type fs_write_time;

    std::filesystem::path pathVS;
    std::filesystem::path pathFS;

    Shaders id;
};

struct Assets {
    static auto* I() {
        std::call_once(Assets::once, [](){
            it.reset(new Assets());
        });
        return it.get();
    }

    inline Assets() {}
    inline ~Assets() {}

    Texture2D textures[TEX_NUM_TEXTURES];
    Font fonts[FONT_NUM_FONTS];

    Shader shaders[SHADER_NUM_SHADERS];
    ShaderAssetInfo shaders_info[SHADER_NUM_SHADERS];

    Item getItemInfo(std::string id);
    Loot getLootInfo(std::string id);

    std::map<std::string, Item> itemDb;
    std::map<std::string, Loot> lootDb;

private:
    Assets(const Assets&) = delete;
    Assets& operator=(const Assets&) = delete;

    static uptr<Assets> it;
    static std::once_flag once;
};

void LoadAllAssets();
void UpdateAssets();
