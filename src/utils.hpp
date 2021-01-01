#ifndef SKYVAULT_UTILS_HPP
#define SKYVAULT_UTILS_HPP

#include <memory>

// Enum tools
#ifndef GENERATE_ENUM
#define GENERATE_ENUM(ENUM) ENUM,
#endif//GENERATE_ENUM

#ifndef GENERATE_STRING
#define GENERATE_STRING(STR) #STR,
#endif//GENERATE_STRING

template <typename T>
using uptr = std::unique_ptr<T>;

template<typename T>
T lerp(T a, T b, T t) {
    return a + (b - a) * t;
}

#endif // SKYVAULT_UTILS_HPP
