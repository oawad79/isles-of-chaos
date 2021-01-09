#ifndef SKYVAULT_CHARACTER_H
#define SKYVAULT_CHARACTER_H

#include <optional>

#include "item.hpp"

/*
** This struct houses the stats and currently equiped items for any character
*/

struct Equiped {
    std::optional<Item> helmet;
    std::optional<Item> chestPiece;
    std::optional<Item> leggings;
    std::optional<Item> boots;
    std::optional<Item> weapon;
};

struct Character {
    Equiped equiped;
};

#endif // SKYVAULT_CHARACTER_H
