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
    E(KiwiBird)       \
    E(Test)            \
    E(Zambie)          \
    E(Ghost)           \
    E(Shroomba)        \
    E(Pot)             \
    E(Water)           \
    E(DreadShark)      \
    E(Chest)           \
    E(Particle)        \
    E(Small_Boat)      \
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

#endif // SKYVAULT_ENTTYPES_HPP
