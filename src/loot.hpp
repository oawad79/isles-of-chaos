#ifndef SKYVAULT_LOOT_HPP
#define SKYVAULT_LOOT_HPP

#include <string>
#include <vector>

#include "utils.hpp"

struct Loot {
  struct Slot {
    std::string itemId{""};
    int chancePercent{100};
    Range1D amount{1, 1};
  };

  std::string id{""};

  std::vector<Slot> slots{};
};

#endif // SKYVAULT_LOOT_HPP
