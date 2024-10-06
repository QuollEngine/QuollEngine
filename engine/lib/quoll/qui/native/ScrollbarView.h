#pragma once

#include "../component/View.h"
#include "../properties/Color.h"

namespace qui {

class ScrollbarView : public View {
public:
  void render() override;

  LayoutOutput layout(const LayoutInput &input) override;

  bool hitTest(const glm::vec2 &point) override;

  constexpr void setThickness(f32 thickness) { mThickness = thickness; }
  constexpr void setBorderRadius(f32 borderRadius) {
    mBorderRadius = borderRadius;
  }
  constexpr void setColor(const Color &color) { mColor = color; }

public:
  constexpr glm::vec2 getPosition() const { return mPosition; }
  constexpr glm::vec2 getSize() const { return mSize; }
  constexpr f32 getThickness() const { return mThickness; }
  constexpr const Color &getColor() const { return mColor; }

private:
  glm::vec2 mPosition{0.0f, 0.0f};
  glm::vec2 mSize{0.0f, 0.0f};

  f32 mThickness = 10.0f;
  f32 mBorderRadius = 6.0f;
  Color mColor = Color::Black;
};

} // namespace qui