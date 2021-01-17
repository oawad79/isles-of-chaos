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

// Defer
template <typename F>
struct DeferStruct {
    F f;
    DeferStruct(F f) : f(f) {}
    ~DeferStruct() { f(); }
};

template <typename F>
DeferStruct<F> DeferFunc(F f) {
    return DeferStruct<F>(f);
}

#define DEFER_1(x,y) x##y
#define DEFER_2(x,y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) auto DEFER_3(_defer_) = DeferFunc([&](){code;})

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
