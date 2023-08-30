#pragma once

namespace quoll {

#define EnableBitwiseEnum(TEnum)                                               \
  constexpr inline TEnum operator~(TEnum a) {                                  \
    using TUnder = typename std::underlying_type_t<TEnum>;                     \
    return static_cast<TEnum>(~static_cast<TUnder>(a));                        \
  }                                                                            \
  constexpr inline TEnum operator|(TEnum a, TEnum b) {                         \
    using TUnder = typename std::underlying_type_t<TEnum>;                     \
    return static_cast<TEnum>(static_cast<TUnder>(a) |                         \
                              static_cast<TUnder>(b));                         \
  }                                                                            \
  constexpr inline TEnum operator&(TEnum a, TEnum b) {                         \
    using TUnder = typename std::underlying_type_t<TEnum>;                     \
    return static_cast<TEnum>(static_cast<TUnder>(a) &                         \
                              static_cast<TUnder>(b));                         \
  }                                                                            \
  constexpr inline TEnum operator^(TEnum a, TEnum b) {                         \
    using TUnder = typename std::underlying_type_t<TEnum>;                     \
    return static_cast<TEnum>(static_cast<TUnder>(a) ^                         \
                              static_cast<TUnder>(b));                         \
  }                                                                            \
  constexpr inline TEnum &operator|=(TEnum &a, TEnum b) {                      \
    using TUnder = typename std::underlying_type_t<TEnum>;                     \
    a = static_cast<TEnum>(static_cast<TUnder>(a) | static_cast<TUnder>(b));   \
    return a;                                                                  \
  }                                                                            \
  constexpr inline TEnum &operator&=(TEnum &a, TEnum b) {                      \
    using TUnder = typename std::underlying_type_t<TEnum>;                     \
    a = static_cast<TEnum>(static_cast<TUnder>(a) & static_cast<TUnder>(b));   \
    return a;                                                                  \
  }                                                                            \
  constexpr inline TEnum &operator^=(TEnum &a, TEnum b) {                      \
    using TUnder = typename std::underlying_type_t<TEnum>;                     \
    a = static_cast<TEnum>(static_cast<TUnder>(a) ^ static_cast<TUnder>(b));   \
    return a;                                                                  \
  }                                                                            \
  constexpr inline bool BitwiseEnumContains(const TEnum &a, TEnum b) {         \
    return (a & b) == b;                                                       \
  }

} // namespace quoll
