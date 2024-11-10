#pragma once

#include "../component/Element.h"
#include "../properties/Color.h"
#include "../properties/EdgeInsets.h"

namespace qui {

class BoxView : public View {
public:
  constexpr void setChild(View *child) { mChild = child; }
  constexpr void setBackground(Color color) { mBackground = color; }
  constexpr void setPadding(EdgeInsets padding) { mPadding = padding; }
  constexpr void setWidth(f32 width) { mWidth = width; }
  constexpr void setHeight(f32 height) { mHeight = height; }
  constexpr void setBorderRadius(f32 radius) { mBorderRadius = radius; }

  void render() override;
  LayoutOutput layout(const LayoutInput &input) override;
  bool hitTest(const glm::vec2 &point, HitTestResult &hitResult) override;

public:
  constexpr auto *getChild() { return mChild; }
  constexpr const auto &getBackground() { return mBackground; }
  constexpr const auto &getPadding() { return mPadding; }
  constexpr f32 getWidth() { return mWidth; }
  constexpr f32 getHeight() { return mHeight; }
  constexpr f32 getBorderRadius() { return mBorderRadius; }
  constexpr glm::vec2 getPosition() { return mPosition; }
  constexpr glm::vec2 getSize() { return mSize; }

private:
  View *mChild = nullptr;
  Color mBackground{Color::Transparent};
  EdgeInsets mPadding;
  f32 mWidth{0};
  f32 mHeight{0};
  f32 mBorderRadius{0};

  glm::vec2 mPosition;
  glm::vec2 mSize;
};

} // namespace qui
