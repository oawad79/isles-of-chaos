#ifndef SKYVAULT_CHARACTER_H
#define SKYVAULT_CHARACTER_H

#include <optional>
#include <entt.hpp>

#include "item.hpp"
#include "inventory.hpp"

/*
** This struct houses the stats and currently equiped items for any character
*/
constexpr auto MAX_HIT_TIMER {0.2f};

enum EquipSlots {
    EQUIP_SLOT_HELMET,
    EQUIP_SLOT_CHEST_PIECE,
    EQUIP_SLOT_LEGGINGS,
    EQUIP_SLOT_BOOTS,
    EQUIP_SLOT_WEAPON,
    EQUIP_SLOT_ABILITY,
};

struct Health {
    int amount{100};
    int max{100};
    float hitTimer{0.0f};
    float maxHitTimer{MAX_HIT_TIMER};

    void hit(float damage);
    void heal(float by);
    bool shouldDie();
    bool canHit();
};

struct Character : Health {
    Inventory equiped;

    Character();

    std::optional<Item> equip(Item item);
    std::optional<Item> unEquip(EquipSlots slot);

    inline bool hasAbility() { return equiped.slots[EquipSlots::EQUIP_SLOT_ABILITY].it.has_value(); }
    inline bool hasWeapon() { return equiped.slots[EquipSlots::EQUIP_SLOT_ABILITY].it.has_value(); }
    inline std::optional<Item> getAbility() { return equiped.slots[EquipSlots::EQUIP_SLOT_ABILITY].it; }
    inline std::optional<Item> getWeapon() { return equiped.slots[EquipSlots::EQUIP_SLOT_WEAPON].it; }
};

void UpdateCharacter(entt::registry& reg);

#endif // SKYVAULT_CHARACTER_H
