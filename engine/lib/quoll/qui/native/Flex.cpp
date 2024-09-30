#include "quoll/core/Base.h"
#include "Flex.h"

namespace qui {

Flex::Flex(Value<std::vector<Element>> children) : mChildren(children) {}

Flex &Flex::direction(Value<Direction> direction) {
  mDirection = direction;
  return *this;
}

Flex &Flex::wrap(Value<Wrap> wrap) {
  mWrap = wrap;
  return *this;
}

Flex &Flex::shrink(Value<f32> shrink) {
  mShrink = shrink;
  return *this;
}

Flex &Flex::grow(Value<f32> grow) {
  mGrow = grow;
  return *this;
}

Flex &Flex::spacing(Value<glm::vec2> spacing) {
  mSpacing = spacing;
  return *this;
}

void Flex::build() {
  auto observeChildren = [this] {
    auto &children = mChildren();
    std::vector<View *> viewChildren(children.size());
    for (usize i = 0; i < children.size(); ++i) {
      children.at(i).build();
      viewChildren.at(i) = children.at(i).getView();
    }

    mView.setChildren(viewChildren);
  };
  auto observeDirection = [this] { mView.setDirection(mDirection()); };
  auto observerWrap = [this] { mView.setWrap(mWrap()); };
  auto observeShrink = [this] { mView.setShrink(mShrink()); };
  auto observeGrow = [this] { mView.setGrow(mGrow()); };
  auto observeSpacing = [this] { mView.setSpacing(mSpacing()); };

  mChildren.observe(observeChildren);
  mDirection.observe(observeDirection);
  mWrap.observe(observerWrap);
  mShrink.observe(observeShrink);
  mGrow.observe(observeGrow);
  mSpacing.observe(observeSpacing);

  observeChildren();
  observeDirection();
  observerWrap();
  observeShrink();
  observeGrow();
  observeSpacing();
}

} // namespace qui
