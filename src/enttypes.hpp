#ifndef SKYVAULT_ENTTYPES_HPP
#define SKYVAULT_ENTTYPES_HPP

#include <vector>
#include <string>
#include <map>

#include "utils.hpp"

#define X_ENT_TYPES(E) \
    E(None)            \
    E(Player)          \
    E(Test)            \
    E(Zambie)          \
    E(Ghost)           \
    E(Pot)             \
    E(Water)           \
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

struct Disabled {};

#undef GENERATE_MAP

#endif // SKYVAULT_ENTTYPES_HPP
