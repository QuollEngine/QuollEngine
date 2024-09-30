#include "quoll/qui/component/View.h"

class MockView : public qui::View {
public:
  constexpr void render() override { rendered++; }

  constexpr qui::LayoutOutput layout(const qui::LayoutInput &input) override {
    this->input = input;

    position = input.position;
    size.x = input.constraints.clampWidth(desiredSize.x);
    size.y = input.constraints.clampHeight(desiredSize.y);

    return {size};
  }

public:
  qui::LayoutInput input;

  glm::vec2 desiredSize;
  glm::vec2 size;
  glm::vec2 position;

  u32 value = 0;
  u32 rendered = 0;
};
