#pragma once

#include "EventDispatcher.h"
#include "HitTestResult.h"

namespace qui {

class Constraints {
public:
  static constexpr f32 Infinity = std::numeric_limits<f32>::infinity();

public:
  glm::vec2 min{0.0f, 0.0f};
  glm::vec2 max{Infinity, Infinity};

public:
  constexpr Constraints() = default;
  constexpr Constraints(f32 minWidth, f32 minHeight, f32 maxWidth,
                        f32 maxHeight)
      : min(minWidth, minHeight), max(maxWidth, maxHeight) {}

  constexpr f32 clampWidth(f32 width) const {
    return std::clamp(width, min.x, max.x);
  }

  constexpr f32 clampHeight(f32 height) const {
    return std::clamp(height, min.y, max.y);
  }

  constexpr glm::vec2 clamp(const glm::vec2 &size) const {
    return glm::clamp(size, min, max);
  }
};

struct LayoutInput {
  Constraints constraints;
  glm::vec2 position{0.0f, 0.0f};
};

struct LayoutOutput {
  glm::vec2 size{0.0f, 0.0f};
};

class View {
public:
  virtual ~View() = default;

  virtual constexpr void render() {}

  virtual LayoutOutput layout(const LayoutInput &input) = 0;

  virtual constexpr bool hitTest(const glm::vec2 &point,
                                 HitTestResult &hitResult) {
    return false;
  }

  constexpr EventDispatcher &getEventDispatcher() { return mEventDispatcher; }

private:
  EventDispatcher mEventDispatcher;
};

} // namespace qui
