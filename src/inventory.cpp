#include "inventory.hpp"

Inventory::Inventory(size_t _maxColumns, size_t _maxRows)
    : maxColumns(_maxColumns)
    , maxRows(_maxRows)
{}

bool Inventory::putItem(Item item){
    for (int r = 0; r < maxRows; r++) {
        for (int c = 0; c < maxColumns; c++) {
            if (getItem(c, r) == std::nullopt) {
                putItemAt(std::optional{item}, c, r);
                return true;
            } else {
                auto itemO = slots[c + r * maxColumns];
                if (itemO.has_value()) {
                    auto slot = itemO.value();

                    if (slot.id == item.id) {
                        slots[c + r * maxColumns].value().amount++;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Inventory::clearItemAt(unsigned int column, unsigned int row) {
    if (column >= maxColumns || row >= maxRows) return false;
    slots[column + row * maxColumns] = std::nullopt;
    return true;
}

bool Inventory::putItemAt(std::optional<Item> item, unsigned int column, unsigned int row) {
    if (column >= maxColumns || row >= maxRows) return false;
    slots[column + row * maxColumns] = item;
    return true;
}

std::optional<Item> Inventory::getItem(unsigned int column, unsigned int row) {
    if (column >= maxColumns || row >= maxRows) return std::nullopt;
    return slots[column + row * maxColumns];
}

void Inventory::decOrClear(unsigned int column, unsigned int row) { 
    if (column >= maxColumns || row >= maxRows) return;

    auto& item = slots[column + row * maxColumns];
    if (item.has_value()) {
        if (item.value().amount == 1) {
            slots[column + row * maxColumns] = std::nullopt;
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
    for (int r = 0; r < maxRows; r++) {
        for (int c = 0; c < maxColumns; c++) {
            slots[c + r * maxColumns] = std::nullopt;
        }
    }
}
