#include "quoll/core/Base.h"
#include "Box.h"

namespace qui {

Box::Box(Value<Element> child) : mChild(child) {}

Box &Box::padding(Value<EdgeInsets> padding) {
  mPadding = padding;
  return *this;
}

Box &Box::background(Value<Color> color) {
  mBackground = color;
  return *this;
}

Box &Box::width(Value<f32> width) {
  mWidth = width;
  return *this;
}

Box &Box::height(Value<f32> height) {
  mHeight = height;
  return *this;
}

Box &Box::borderRadius(Value<f32> radius) {
  mBorderRadius = radius;
  return *this;
}

void Box::build() {
  auto observeChild = [this] {
    if (mChild()) {
      mChild().build();
      mView.setChild(mChild().getView());
    }
  };

  auto observeBackground = [this] { mView.setBackground(mBackground()); };
  auto observePadding = [this] { mView.setPadding(mPadding()); };
  auto observeWidth = [this] { mView.setWidth(mWidth()); };
  auto observeHeight = [this] { mView.setHeight(mHeight()); };
  auto observeBorderRadius = [this] { mView.setBorderRadius(mBorderRadius()); };

  mChild.observe(observeChild);
  mBackground.observe(observeBackground);
  mPadding.observe(observePadding);
  mWidth.observe(observeWidth);
  mHeight.observe(observeHeight);
  mBorderRadius.observe(observeBorderRadius);

  observeChild();
  observeBackground();
  observePadding();
  observeWidth();
  observeHeight();
  observeBorderRadius();
}

} // namespace qui
