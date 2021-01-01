#ifndef SKYVAULT_ENTTYPES_HPP
#define SKYVAULT_ENTTYPES_HPP

#include <vector>
#include <map>

#include "utils.hpp"

#define X_ENT_TYPES(E) \
    E(Player)          \
    E(Test)

enum class EntTypes {X_ENT_TYPES(GENERATE_ENUM)};
#define GENERATE_MAP(V) { #V, EntTypes::V },

static const std::vector<std::string> EntTypesS = {X_ENT_TYPES(GENERATE_STRING)};
static std::map<std::string, EntTypes> EntTypesM = {
    X_ENT_TYPES(GENERATE_MAP)
};

#undef GENERATE_MAP

#endif // SKYVAULT_ENTTYPES_HPP
