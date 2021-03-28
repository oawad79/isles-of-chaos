#ifndef SKYVAULT_CHARACTER_H
#define SKYVAULT_CHARACTER_H

#include <optional>
#include <entt.hpp>

#include "item.hpp"

/*
** This struct houses the stats and currently equiped items for any character
*/
constexpr auto MAX_HIT_TIMER {0.2f};

struct Equiped {
    std::optional<Item> helmet;
    std::optional<Item> chestPiece;
    std::optional<Item> leggings;
    std::optional<Item> boots;
    std::optional<Item> weapon;
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
    Equiped equiped;
};

void UpdateCharacter(entt::registry& reg);

#endif // SKYVAULT_CHARACTER_H
