#ifndef SKYVAULT_INVENTORY_HPP
#define SKYVAULT_INVENTORY_HPP

#include "item.hpp"

#include <array>
#include <optional>

constexpr auto MAX_INV_COLUMNS {30};
constexpr auto MAX_INV_ROWS {20};

struct Slot {
  int column{0};
  int row{0};
  std::optional<Item> it;
};

struct Inventory {
    Inventory(size_t _maxColumns=5, size_t _maxRows=3);

    std::vector<Slot> slots;

    size_t maxColumns{5};
    size_t maxRows{3};

    bool putItem(Item item);
    bool clearItemAt(unsigned int column, unsigned int row);
    bool putItemAt(const std::optional<Item>& item, unsigned int column, unsigned int row);
    std::optional<Item> getItem(int index);
    void grow(size_t newMaxColumns, size_t newMaxRows);

    void decOrClear(unsigned int column, unsigned int row);

    void clear();

    inline int getIndex(int x, int y) {
      return x + y * maxColumns;
    }
};

#endif // SKYVAULT_INVENTORY_HPP
