#pragma once

#include "../component/Element.h"
#include "../reactive/Value.h"
#include "FlexView.h"

namespace qui {

class Flex : public Component {
public:
  Flex(Value<std::vector<Element>> children);

  Flex &direction(Value<Direction> direction);
  Flex &wrap(Value<Wrap> wrap);
  Flex &shrink(Value<f32> shrink);
  Flex &grow(Value<f32> grow);
  Flex &spacing(Value<glm::vec2> spacing);

  void build(BuildContext &context) override;

  View *getView() override { return &mView; }

public:
  constexpr auto getChildren() const { return mChildren(); }
  constexpr auto getDirection() const { return mDirection(); }
  constexpr auto getWrap() const { return mWrap(); }
  constexpr auto getShrink() const { return mShrink(); }
  constexpr auto getGrow() const { return mGrow(); }
  constexpr auto getSpacing() const { return mSpacing(); }

private:
  FlexView mView;
  Value<std::vector<Element>> mChildren;
  Value<Direction> mDirection = Direction::Row;
  Value<Wrap> mWrap = Wrap::NoWrap;
  Value<f32> mShrink = 1.0f;
  Value<f32> mGrow = 0.0f;
  Value<glm::vec2> mSpacing = glm::vec2(0.0f);
};

} // namespace qui
