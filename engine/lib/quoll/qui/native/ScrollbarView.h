#pragma once

#include "../component/View.h"
#include "../properties/Color.h"

namespace qui {

class ScrollbarView : public View {
public:
  void render() override;

  void layout(const LayoutInput &input) override;

  bool hitTest(const glm::vec2 &point, HitTestResult &hitResult) override;

  constexpr void setThickness(f32 thickness) { mThickness = thickness; }
  constexpr void setBorderRadius(f32 borderRadius) {
    mBorderRadius = borderRadius;
  }
  constexpr void setColor(const Color &color) { mColor = color; }

public:
  constexpr f32 getThickness() const { return mThickness; }
  constexpr const Color &getColor() const { return mColor; }

private:
  f32 mThickness = 10.0f;
  f32 mBorderRadius = 6.0f;
  Color mColor = Color::Black;
};

} // namespace qui