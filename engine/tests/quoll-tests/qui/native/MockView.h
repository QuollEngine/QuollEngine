#include "quoll/qui/component/View.h"

class MockView : public qui::View {
public:
  constexpr void render() override { rendered++; }

  constexpr void layout(const qui::LayoutInput &input) override {
    this->input = input;

    mPosition = input.position;
    mSize.x = input.constraints.clampWidth(desiredSize.x);
    mSize.y = input.constraints.clampHeight(desiredSize.y);
  }

  constexpr bool hitTest(const glm::vec2 &pos,
                         qui::HitTestResult &hitResult) override {
    const auto end = mPosition + mSize;
    if (pos.x >= mPosition.x && pos.x <= end.x && pos.y >= mPosition.y &&
        pos.y <= end.y) {
      hitResult.path.push_back(this);
      return true;
    }

    return false;
  }

public:
  qui::LayoutInput input;

  glm::vec2 desiredSize;

  u32 value = 0;
  u32 rendered = 0;
};
