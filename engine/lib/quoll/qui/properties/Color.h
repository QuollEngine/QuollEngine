#pragma once

namespace qui {

class Color {
public:
  glm::vec4 value{0.0f, 0.0f, 0.0f, 1.0f};

public:
  static const Color White;
  static const Color Black;
  static const Color Red;
  static const Color Green;
  static const Color Blue;

public:
  constexpr Color() = default;
  constexpr Color(f32 r, f32 g, f32 b, f32 a = 1.0f) : value(r, g, b, a) {}
  constexpr Color(u32 r, u32 g, u32 b, u32 a = 255)
      : value(normalize(r), normalize(g), normalize(b), normalize(a)) {}
  constexpr Color(u32 color)
      : value(normalize((color >> 16) & 0xFF), normalize((color >> 8) & 0xFF),
              normalize(color & 0xFF), normalize((color >> 24) & 0xFF)) {}

  constexpr bool operator==(const Color &rhs) const {
    return value == rhs.value;
  }

  constexpr bool operator!=(const Color &rhs) const { return !(*this == rhs); }

public:
  static constexpr f32 normalize(u32 value) {
    return static_cast<f32>(value) / 255.0f;
  }
};

} // namespace qui
