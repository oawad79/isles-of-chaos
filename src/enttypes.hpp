#ifndef SKYVAULT_ENTTYPES_HPP
#define SKYVAULT_ENTTYPES_HPP

#include <vector>
#include <string>
#include <map>

#include "utils.hpp"

#define X_ENT_TYPES(E) \
    E(None)            \
    E(Npc)             \
    E(Player)          \
    E(KiwiBird)        \
    E(ShopKeeper)      \
    E(Test)            \
    E(Zambie)          \
    E(Ghost)           \
    E(Shroomba)        \
    E(Pot)             \
    E(DreadShark)      \
    E(Mimic)           \
    E(Chest)           \
    E(Particle)        \
    E(SmallBoat)       \
    E(SmallWorm)       \
    E(Item)

enum class EntType {X_ENT_TYPES(GENERATE_ENUM)};
#define GENERATE_MAP(V) { #V, EntType::V },

static const std::vector<std::string> EntTypeS = {X_ENT_TYPES(GENERATE_STRING)};
static std::map<std::string, EntType> EntTypeM = {
    X_ENT_TYPES(GENERATE_MAP)
};

static EntType GetEntType(const std::string& tstr) {
    if (EntTypeM.find(tstr) == EntTypeM.end()) {
        return EntType::None;
    }

    return EntTypeM[tstr];
}

struct Disabled { int mapId {0}; };

#undef GENERATE_MAP

enum class ActorName {
    Anon,
    Player,
    OldMan,
};

struct Ent {
  EntType entType{EntType::None};
};

#endif // SKYVAULT_ENTTYPES_HPP
