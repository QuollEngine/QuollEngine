#pragma once

#include "../component/View.h"
#include "FlexProps.h"

namespace qui {

class FlexView : public View {
public:
  constexpr void setDirection(Direction direction) { mDirection = direction; }
  constexpr void setWrap(Wrap wrap) { mWrap = wrap; }
  constexpr void setSpacing(const glm::vec2 &spacing) { mSpacing = spacing; }
  constexpr void setChildren(const std::vector<View *> &children) {
    mChildren = children;
  }
  constexpr void setShrink(f32 shrink) { mShrink = shrink; }
  constexpr void setGrow(f32 grow) { mGrow = grow; }

  void render() override;

  LayoutOutput layout(const LayoutInput &input) override;

  bool hitTest(const glm::vec2 &point) override;

public:
  constexpr auto getDirection() const { return mDirection; }
  constexpr auto getWrap() const { return mWrap; }
  constexpr auto getSpacing() const { return mSpacing; }
  constexpr auto getChildren() const { return mChildren; }
  constexpr auto getShrink() const { return mShrink; }
  constexpr auto getGrow() const { return mGrow; }

  constexpr auto getSize() const { return mSize; }
  constexpr auto getPosition() const { return mPosition; }

private:
  Direction mDirection{Direction::Row};
  Wrap mWrap{Wrap::NoWrap};
  f32 mShrink = 1.0f;
  f32 mGrow = 0.0f;
  glm::vec2 mSpacing{0.0f, 0.0f};
  std::vector<View *> mChildren;

  glm::vec2 mSize{0.0f, 0.0f};
  glm::vec2 mPosition{0.0f, 0.0f};
};

} // namespace qui
