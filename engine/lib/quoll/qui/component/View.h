#pragma once

namespace qui {

class Constraints {
public:
  f32 minHeight = 0.0f;
  f32 minWidth = 0.0f;
  f32 maxHeight = 0.0f;
  f32 maxWidth = 0.0f;

public:
  constexpr f32 clampWidth(f32 width) const {
    return std::clamp(width, minWidth, maxWidth);
  }

  constexpr f32 clampHeight(f32 height) const {
    return std::clamp(height, minHeight, maxHeight);
  }
};

struct LayoutInput {
  Constraints constraints;
  glm::vec2 position;
};

struct LayoutOutput {
  glm::vec2 size;
};

class View {
public:
  virtual ~View() = default;

  virtual LayoutOutput layout(const LayoutInput &input) = 0;
};

} // namespace qui
