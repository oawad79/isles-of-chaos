#include "assets.hpp"

void LoadAllAssets() {

    // Load Textures
    Assets::I()->textures[Textures::TEX_OVERWORLD] = LoadTexture("resources/textures/OverworldTileset.png");
    Assets::I()->textures[Textures::TEX_GUI] = LoadTexture("resources/textures/Gui.png");

    // Assets::I()->fonts[Fonts::FONT_DIALOG_1] = LoadFont("resources/fonts/alpha_beta.png");

    Assets::I()->fonts[Fonts::FONT_NONE] = GetFontDefault();

    // auto loadDynShader = [&](Shaders which, const char* pathVS, const char* pathFS) {
    //     Assets::I()->shaders[which] = LoadShader(pathVS, pathFS);
    //     Assets::I()->shaders_info[which] = {
    //         std::filesystem::last_write_time(pathVS),
    //         std::filesystem::last_write_time(pathFS),
    //         pathVS, pathFS, which
    //     };
    // };

    // loadDynShader(Shaders::SHADER_PLANETS, "resources/shaders/base_vs.glsl", "resources/shaders/planet_fs.glsl");
    // loadDynShader(Shaders::SHADER_SKY, "resources/shaders/sky_vs.glsl", "resources/shaders/sky_fs.glsl");
    // loadDynShader(Shaders::SHADER_SPACE, "resources/shaders/sky_vs.glsl", "resources/shaders/space_fs.glsl");
    // loadDynShader(Shaders::SHADER_CHARACTERS, "resources/shaders/base_vs.glsl", "resources/shaders/characters_fs.glsl");
    // loadDynShader(Shaders::SHADER_MAIN_SHADING, "resources/shaders/base_vs.glsl", "resources/shaders/main_shading_fs.glsl");
}

void UpdateAssets() {
}
