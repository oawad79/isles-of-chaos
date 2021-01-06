#include "inventory.hpp"

Inventory::Inventory(size_t _maxColumns, size_t _maxRows)
    : maxColumns(_maxColumns)
    , maxRows(_maxRows)
{}

bool Inventory::putItem(Item item){
    for (int r = 0; r < maxRows; r++) {
        for (int c = 0; c < maxColumns; c++) {
            if (getItem(c, r) == std::nullopt) {
                putItemAt(item, c, r);
                return true;
            }
        }
    }
    return false;
}

bool Inventory::putItemAt(Item item, unsigned int column, unsigned int row) {
    if (column >= maxColumns || row >= maxRows) return false;
    slots[column + row * maxColumns] = std::optional{item};
    return true;
}

std::optional<Item> Inventory::getItem(unsigned int column, unsigned int row) {
    if (column >= maxColumns || row >= maxRows) return std::nullopt;
    return slots[column + row * maxColumns];
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
