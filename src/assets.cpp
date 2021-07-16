#include "assets.hpp"

Item Assets::getItemInfo(std::string id) {
    if (itemDb.find(id) == itemDb.end()) {
        std::cout << "WARNING:: Cannot find item with id: '" << id << "'\n";
        return Item();
    }
    return itemDb[id];
}

Loot Assets::getLootInfo(std::string id) {
    if (lootDb.find(id) == lootDb.end()) {
        std::cout << "WARNING:: Cannot find loot set with id: '" << id << "'\n";
        return Loot();
    }
    return lootDb[id];
}

void LoadItemDB() {
    using namespace tinyxml2;

    XMLDocument doc;
    doc.LoadFile("resources/data/itemdb.xml");
    auto* root = doc.FirstChild();
    auto* items = root->NextSiblingElement();

    auto* consumables = items->FirstChildElement("consumables");
    auto* weapons = items->FirstChildElement("weapons");
    auto* armor = items->FirstChildElement("armor");
    auto* moneys = items->FirstChildElement("moneys");
    auto* abilities = items->FirstChildElement("abilities");

    auto loadItem = [&](XMLElement* itemXml, ItemCatagory catagory) {
        Item item;
        item.catagory = catagory;
        item.id = std::string{itemXml->Attribute("id")};
        item.name = std::string{itemXml->Attribute("name")};
        item.descr = std::string{itemXml->Attribute("descr")};

        itemXml->QueryFloatAttribute("effectValue", &item.effectValue);
        itemXml->QueryIntAttribute("value", &item.value);
        itemXml->QueryFloatAttribute("cooloff", &item.usageCooloff);

        std::stringstream ss(itemXml->Attribute("region"));
        ss >> item.region.x;
        ss >> item.region.y;
        ss >> item.region.width;
        ss >> item.region.height;
 
        if (catagory == ItemCatagory::Consumable) {
            std::string consumableEffect
                = std::string{itemXml->Attribute("consumableEffect")};
            if (ConsumableEffectM.find(consumableEffect) ==
                ConsumableEffectM.end()) {
                std::cout << "ERROR::LoadItemDB:: Invalid consumable effect: " << consumableEffect << std::endl;
            } else {
                item.consumableEffect = ConsumableEffectM[consumableEffect];
            }
        } else if (catagory == ItemCatagory::Weapon) {
            std::string weaponClass
                = std::string{itemXml->Attribute("weaponClass")};
            if (WeaponClassM.find(weaponClass) == WeaponClassM.end()) {
                std::cout << "ERROR::LoadItemDB:: Invalid weapon class: "
                          << weaponClass
                          << std::endl;
            } else {
                item.weaponClass = WeaponClassM[weaponClass];
            }
            item.flags |= ITEM_FLAG_BOUNCE;
        }

        Assets::I()->itemDb[item.id] = item;
    };

    // Loading consumables
    if (consumables) {
        auto* itemXml = consumables->FirstChildElement("item");
        while (itemXml) {
            loadItem(itemXml, ItemCatagory::Consumable);
            itemXml = itemXml->NextSiblingElement();
        }
    }

    // Weapons
    if (weapons) {
        auto* itemXml = weapons->FirstChildElement("item");
        while (itemXml) {
            loadItem(itemXml, ItemCatagory::Weapon);
            itemXml = itemXml->NextSiblingElement();
        }
    }

    // Moneys
    if (moneys) {
        auto* itemXml = moneys->FirstChildElement("item");
        while (itemXml) {
            loadItem(itemXml, ItemCatagory::Money);
            itemXml = itemXml->NextSiblingElement();
        }
    }

    // Abilities
    if (abilities) {
        auto* itemXml = abilities->FirstChildElement("item");
        while (itemXml) {
            loadItem(itemXml, ItemCatagory::Ability);
            itemXml = itemXml->NextSiblingElement();
        }
    }
}

void LoadLootDB() {
    using namespace tinyxml2;

    XMLDocument doc;
    doc.LoadFile("resources/data/lootsets.xml");

    auto* root = doc.FirstChild();
    auto* chestLootSets = root->NextSiblingElement("chest-loot-sets");

    auto lootXml = chestLootSets->FirstChildElement("chest-loot");
    while (lootXml) {
        Loot loot;
        loot.id = std::string{lootXml->Attribute("id")};

        auto* itemDesc = lootXml->FirstChildElement();
        while (itemDesc) {
            Loot::Slot slot;
            slot.itemId = std::string{itemDesc->Attribute("itemId")};

            if (itemDesc->FindAttribute("chance")) {
                itemDesc->QueryIntAttribute("chance", &slot.chancePercent);
            }

            if (itemDesc->FindAttribute("amount")) {
                const char* amountS = itemDesc->Attribute("amount");
                std::stringstream ss(std::string{amountS});
                ss >> slot.amount.min;
                ss >> slot.amount.max;
                std::cout << "MIN::" << slot.amount.min << " MAX::" << slot.amount.max << std::endl;
            }

            loot.slots.emplace_back(slot);
            itemDesc = itemDesc->NextSiblingElement();
        }

        Assets::I()->lootDb[loot.id] = loot;
        lootXml = lootXml->NextSiblingElement();
    }
}

void LoadTexture(Textures which, const char* path) {
    Assets::I()->textures[which] = LoadTexture(path);
}

void LoadAllAssets() {
    LoadItemDB();
    LoadLootDB();

    // Load Textures
    Assets::I()->textures[Textures::TEX_OVERWORLD] = LoadTexture("resources/textures/OverworldTileset.png");
    Assets::I()->textures[Textures::TEX_BG] = LoadTexture("resources/textures/Background1.png");
    Assets::I()->textures[Textures::TEX_GUI] = LoadTexture("resources/textures/Gui.png");
    Assets::I()->textures[Textures::TEX_EQUIPMENT] = LoadTexture("resources/textures/Equipment.png");
    Assets::I()->textures[Textures::TEX_PLAYER] = LoadTexture("resources/textures/Player.png");
    Assets::I()->textures[Textures::TEX_ENTITIES] = LoadTexture("resources/textures/Entities.png");
    Assets::I()->textures[Textures::TEX_ITEMS] = LoadTexture("resources/textures/Items.png");
    Assets::I()->textures[Textures::TEX_PARTICLES] = LoadTexture("resources/textures/Particles.png");

    // Assets::I()->fonts[Fonts::FONT_DIALOG_1] = LoadFont("resources/fonts/alpha_beta.png");

    Assets::I()->fonts[Fonts::FONT_NONE] = GetFontDefault();

    auto loadDynShader = [&](Shaders which, std::string pathVS, std::string pathFS) {
      Assets::I()->shaders[which] = LoadShader(pathVS.c_str(), pathFS.c_str());
      Assets::I()->shaders_info[which] = {
              std::filesystem::last_write_time(pathVS),
              std::filesystem::last_write_time(pathFS),
              pathVS, pathFS,
              which
      };
    };

    loadDynShader(SPRITE_SHADER, "resources/shaders/base.vs.glsl", "resources/shaders/base.fs.glsl");
    loadDynShader(WATER_SHADER, "resources/shaders/water.vs.glsl", "resources/shaders/water.fs.glsl");
}

void UpdateAssets() {
  static float timer = 0;
  timer += GetFrameTime();

  if (timer > 0.2f) {
    timer = 0;
  } else {
    return;
  }

  for (int i = 0; i < SHADER_NUM_SHADERS; i++) {
    auto& info = Assets::I()->shaders_info[i];

    const auto vs_time = std::filesystem::last_write_time(info.pathVS);
    const auto fs_time = std::filesystem::last_write_time(info.pathFS);

    bool reload = false;

    if (vs_time != info.vs_write_time) {
      std::cout << "Reloading shader " << info.pathVS << std::endl;
      info.vs_write_time = vs_time;
      reload = true;
    }

    if (fs_time != info.fs_write_time) {
      std::cout << "Reloading shader " << info.pathFS << std::endl;
      info.fs_write_time = fs_time;
      reload = true;
    }

    if (reload) {
      UnloadShader(Assets::I()->shaders[i]);
      Assets::I()->shaders[i] = LoadShader(info.pathVS.string().c_str(), info.pathFS.string().c_str());
      Assets::I()->shaders[i].locs[SHADER_RELOAD_SLOT] = 1;
    } else {
      Assets::I()->shaders[i].locs[SHADER_RELOAD_SLOT] = -1;
    }
  }
}
