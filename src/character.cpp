#include "character.hpp"
#include "enttypes.hpp"

void Health::hit(float damage) {
  if (!canHit()) return;
  amount -= damage;
  hitTimer = maxHitTimer;
}

void Health::heal(float by) {
  amount += by;
}

bool Health::shouldDie() {
  return amount <= 0;
}

bool Health::canHit() {
  return hitTimer <= 0;
}

Character::Character() {
  equiped.slots.emplace_back(Slot{-1, 0, std::nullopt});
  equiped.slots.emplace_back(Slot{-1, 1, std::nullopt});
  equiped.slots.emplace_back(Slot{-1, 2, std::nullopt});
  equiped.slots.emplace_back(Slot{-1, 3, std::nullopt});
  equiped.slots.emplace_back(Slot{-2, 1, std::nullopt});
  equiped.slots.emplace_back(Slot{-2, 2, std::nullopt});
  equiped.maxColumns = 1;
  equiped.maxRows = equiped.slots.size();
}

std::optional<Item> Character::equip(Item item) {
  if (item.catagory == ItemCatagory::Weapon) {
    auto ret = equiped.slots[EquipSlots::EQUIP_SLOT_WEAPON].it;
    equiped.slots[EquipSlots::EQUIP_SLOT_WEAPON].it = item;
    return ret;
  }

  if (item.catagory == ItemCatagory::Ability) {
    auto ret = equiped.slots[EquipSlots::EQUIP_SLOT_ABILITY].it;
    equiped.slots[EquipSlots::EQUIP_SLOT_ABILITY].it = item;
    return ret;
  }

  if (item.armorClass > ArmorClass::None && item.armorClass <= ArmorClass::Boots) {
    auto ret = equiped.slots[static_cast<size_t>(item.armorClass)-1].it;
    equiped.slots[static_cast<size_t>(item.armorClass)-1].it = item;
    return ret;
  }

  return std::nullopt;
}

std::optional<Item> Character::unEquip(EquipSlots slot) {
  const auto &item = equiped.slots[slot].it;
  equiped.slots[slot].it = std::nullopt;
  return item;
}

void UpdateCharacter(entt::registry &reg) {
  auto health_ents = reg.view<Health>(entt::exclude<Disabled>);

  for (auto &ent : health_ents) {
    auto &health = reg.get<Health>(ent);
    if (health.shouldDie()) reg.destroy(ent);
    else
      health.hitTimer -= GetFrameTime();
  }
}
