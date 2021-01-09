#ifndef SKYVAULT_INVENTORY_HPP
#define SKYVAULT_INVENTORY_HPP

#include "item.hpp"

#include <vector>
#include <optional>

constexpr auto MAX_INV_COLUMNS {30};
constexpr auto MAX_INV_ROWS {20};

struct Inventory {
    Inventory(size_t _maxColumns=5, size_t _maxRows=3);

    std::array<std::optional<Item>, MAX_INV_COLUMNS*MAX_INV_ROWS> slots;
    size_t maxColumns{5};
    size_t maxRows{3};

    bool putItem(Item item);
    bool clearItemAt(unsigned int column, unsigned int row);
    bool putItemAt(std::optional<Item> item, unsigned int column, unsigned int row);
    std::optional<Item> getItem(unsigned int column, unsigned int row);
    void grow(size_t newMaxColumns, size_t newMaxRows);
    void clear();
};

#endif // SKYVAULT_INVENTORY_HPP
