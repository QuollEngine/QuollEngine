#pragma once

#include "../component/View.h"
#include "../properties/Color.h"

namespace qui {

class TextView : public View {
public:
  constexpr void setText(quoll::String text) { mText = text; }
  constexpr void setColor(Color color) { mColor = color; }

  void render() override;
  LayoutOutput layout(const LayoutInput &input) override;
  bool hitTest(const glm::vec2 &point, HitTestResult &hitResult) override;

public:
  constexpr const auto &getText() { return mText; }
  constexpr const auto &getColor() { return mColor; }
  constexpr glm::vec2 getPosition() { return mPosition; }

private:
  quoll::String mText;
  Color mColor;

  glm::vec2 mTextSize;
  glm::vec2 mPosition;
};

} // namespace qui
