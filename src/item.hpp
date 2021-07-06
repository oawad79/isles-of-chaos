#ifndef SKYVAULT_ITEM_HPP
#define SKYVAULT_ITEM_HPP

#include <raylib.h>

#include <string>
#include <vector>
#include <cstdint>
#include <map>

#include "utils.hpp"

#define X_ITEM_CATAGORY(E)\
    E(None)               \
    E(Money)              \
    E(Consumable)         \
    E(Armor)              \
    E(Ability)            \
    E(Weapon)

#define X_WEAPON_CLASS(E)\
    E(None)              \
    E(Sword)             \
    E(Dagger)            \
    E(Spear)             \
    E(Whip)              \
    E(Bow)

#define X_ARMOR_CLASS(E)\
    E(None)             \
    E(Helmet)           \
    E(ChestPiece)       \
    E(Leggings)         \
    E(Boots)

#define X_CONSUMABLE_EFFECT(E)\
    E(None)                   \
    E(Heal)                   \
    E(ArmorIncrease)

enum class ItemCatagory {X_ITEM_CATAGORY(GENERATE_ENUM)};
static std::vector<std::string> ItemCatagoryS = { X_ITEM_CATAGORY(GENERATE_STRING) };
#define GENERATE_MAP(V) { #V, ItemCatagory::V },
static std::map<std::string, ItemCatagory> ItemCatagoryM = {
  X_ITEM_CATAGORY(GENERATE_MAP)
};
#undef GENERATE_MAP

enum class WeaponClass {X_WEAPON_CLASS(GENERATE_ENUM)};
static std::vector<std::string> WeaponClassS = { X_WEAPON_CLASS(GENERATE_STRING) };
#define GENERATE_MAP(V) { #V, WeaponClass::V },
static std::map<std::string, WeaponClass> WeaponClassM = {
  X_WEAPON_CLASS(GENERATE_MAP)
};
#undef GENERATE_MAP

enum class ArmorClass {X_ARMOR_CLASS(GENERATE_ENUM)};
static std::vector<std::string> ArmorClassS = { X_ARMOR_CLASS(GENERATE_STRING) };
#define GENERATE_MAP(V) { #V, ArmorClass::V },
static std::map<std::string, ArmorClass> ArmorClassM = {
  X_ARMOR_CLASS(GENERATE_MAP)
};
#undef GENERATE_MAP

enum class ConsumableEffect {X_CONSUMABLE_EFFECT(GENERATE_ENUM)};
static std::vector<std::string> ConsumableEffectS = { X_CONSUMABLE_EFFECT(GENERATE_STRING) };
#define GENERATE_MAP(V) { #V, ConsumableEffect::V },
static std::map<std::string, ConsumableEffect>  ConsumableEffectM = {
  X_CONSUMABLE_EFFECT(GENERATE_MAP)
};
#undef GENERATE_MAP

enum ItemFlags {
    ITEM_FLAG_NONE   = 1 << 0,
    ITEM_FLAG_BOUNCE = 1 << 1,
};

struct Item {
    std::string id{"item"};

    std::string name{"Item"}, descr{"An item"};

    Rectangle region{0, 0, 8, 8};

    ItemCatagory catagory{ItemCatagory::None};
    WeaponClass weaponClass{WeaponClass::None};
    ArmorClass armorClass{ArmorClass::None};
    ConsumableEffect consumableEffect{ConsumableEffect::None};

    float effectValue{1.0f};

    int value{1};
    uint32_t flags{0};

    float usageCooloff{0.0f};

    int amount{1}; // Items can also be a stack
};

#endif // SKYVAULT_ITEM_HPP
