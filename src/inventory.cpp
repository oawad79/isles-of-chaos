#include "inventory.hpp"

Inventory::Inventory() {}

Inventory::Inventory(size_t _maxColumns, size_t _maxRows)
    : maxColumns(_maxColumns)
    , maxRows(_maxRows)
{
  for (int y = 0; y < maxRows; y++) {
    for (int x = 0; x < maxColumns; x++) {
      slots.emplace_back(Slot{x, y, std::nullopt});
    }
  }
}

bool Inventory::putItem(Item item){
  auto intersects = [&](Slot& slot) -> bool {
    const auto width = slot.it ? slot.it->width : 1;
    const auto height = slot.it ? slot.it->height : 1;
    for (const auto &other : slots) {
      if (!other.it) continue;
      if (
        slot.column + width >= other.column && slot.column < other.column + other.it->width &&
        slot.row + height >= other.row && slot.row < other.row + other.it->height
      ) {
        return true;
      }
    }

    return false;
  };

  for (auto& slot : slots) {
    if (slot.it == std::nullopt && !intersects(slot)) {

      if (slot.column + item.width > maxColumns) {
        continue;
      } else if (slot.row + item.height > maxRows) {
        continue;
      }

      slot.it = item;
      return true;
    } else {
      if (slot.it->id == item.id) {
        slot.it->amount += 1;
        return true;
      }
    }
  }
  return false;
}

bool Inventory::clearItemAt(unsigned int column, unsigned int row) {
  if (column + row * maxColumns > slots.size()) return false;
    slots[column + row * maxColumns].it = std::nullopt;
    return true;
}

bool Inventory::putItemAt(const std::optional<Item>& item, unsigned int column, unsigned int row) {
    if (column + row * maxColumns > slots.size()) return false;
    slots[column + row * maxColumns].it = item;
    return true;
}

std::optional<Item> Inventory::getItem(int index) {
    if (index < 0 || index > slots.size()) return std::nullopt;
    return slots[index].it;
}

void Inventory::decOrClear(unsigned int column, unsigned int row) {
    if (column + row * maxColumns) return;

    auto& item = slots[column + row * maxColumns].it;
    if (item.has_value()) {
        if (item.value().amount == 1) {
            slots[column + row * maxColumns].it = std::nullopt;
        } else {
            item.value().amount -= 1;
        }
    }
}

void Inventory::grow(size_t newMaxColumns, size_t newMaxRows) {
    maxColumns = newMaxColumns;
    maxRows = newMaxRows;
}

void Inventory::clear(){
  slots.clear();
}
