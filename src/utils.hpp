#ifndef SKYVAULT_UTILS_HPP
#define SKYVAULT_UTILS_HPP

#include <memory>
#include <raylib.h>
#include <string>

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

inline Color ColorFromHexString(const std::string& hexStr) {
    const int start = hexStr[0] == '#' ? 1 : 0;
    auto rs = hexStr.substr(start + 0, start + 1);
    auto gs = hexStr.substr(start + 2, start + 3);
    auto bs = hexStr.substr(start + 4, start + 5);
    unsigned char r = (unsigned char)strtol(rs.c_str(), NULL, 16);
    unsigned char g = (unsigned char)strtol(gs.c_str(), NULL, 16);
    unsigned char b = (unsigned char)strtol(bs.c_str(), NULL, 16);
    return Color{r, g, b, 255};
}

#endif // SKYVAULT_UTILS_HPP
