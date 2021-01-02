#pragma once

#include <memory>
#include <mutex>
#include <algorithm>
#include <cassert>
#include <map>
#include <vector>
#include <raylib.h>
#include <filesystem>
#include <chrono>

#include "utils.hpp"

enum Textures {
    TEX_NONE,
    TEX_BG,
    TEX_GUI,
    TEX_ENTITIES,
    TEX_PLAYER,
    TEX_OVERWORLD,
    TEX_NUM_TEXTURES
};

enum Fonts {
    FONT_NONE,
    FONT_NUM_FONTS,
};

enum Shaders {
    SHADER_NUM_SHADERS
};

struct ShaderAssetInfo {
    std::filesystem::file_time_type last_write_time_1;
    std::filesystem::file_time_type last_write_time_2;

    std::filesystem::path path1;
    std::filesystem::path path2;

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

private:
    Assets(const Assets&) = delete;
    Assets& operator=(const Assets&) = delete;

    static uptr<Assets> it;
    static std::once_flag once;
};

void LoadAllAssets();
void UpdateAssets();
