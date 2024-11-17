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

  virtual void layout(const LayoutInput &input) = 0;

  virtual constexpr bool hitTest(const glm::vec2 &point,
                                 HitTestResult &hitResult) {
    return false;
  }

  constexpr EventDispatcher &getEvents() { return mEvents; }

  constexpr const glm::vec2 &getPosition() const { return mPosition; }
  constexpr const glm::vec2 &getSize() const { return mSize; }

protected:
  constexpr bool isPointInBounds(const glm::vec2 &point) const {
    return point.x >= mPosition.x && point.x <= mPosition.x + mSize.x &&
           point.y >= mPosition.y && point.y <= mPosition.y + mSize.y;
  }

private:
  EventDispatcher mEvents;

protected:
  glm::vec2 mPosition{0.0f};
  glm::vec2 mSize{0.0f};
};

} // namespace qui
