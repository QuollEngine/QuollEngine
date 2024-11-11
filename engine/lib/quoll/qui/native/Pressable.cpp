#include "quoll/core/Base.h"
#include "Pressable.h"

namespace qui {

Pressable::Pressable(Value<Element> child) : mChild(child) {}

Pressable &Pressable::onPress(PressHandler &&handler) {
  mOnPress = std::move(handler);
  return *this;
}

Pressable &Pressable::onPressDown(PressHandler &&handler) {
  mOnPressDown = std::move(handler);
  return *this;
}

Pressable &Pressable::onPressUp(PressHandler &&handler) {
  mOnPressUp = std::move(handler);
  return *this;
}

Pressable &Pressable::onHoverIn(PressHandler &&handler) {
  mOnHoverIn = std::move(handler);
  return *this;
}

Pressable &Pressable::onHoverOut(PressHandler &&handler) {
  mOnHoverOut = std::move(handler);
  return *this;
}

void Pressable::build(BuildContext &context) {
  auto observeChild = [this, &context]() {
    mChild().build(context);

    auto &dispatcher = mChild().getView()->getEventDispatcher();

    if (mOnPress) {
      dispatcher.registerMouseClickHandler(
          [this](const auto &e) { mOnPress({.point = e.point}); });
    }

    if (mOnPressDown) {
      dispatcher.registerMouseDownHandler(
          [this](const auto &e) { mOnPressDown({.point = e.point}); });
    }

    if (mOnPressUp) {
      dispatcher.registerMouseUpHandler(
          [this](const auto &e) { mOnPressUp({.point = e.point}); });
    }

    if (mOnHoverIn) {
      dispatcher.registerMouseEnterHandler(
          [this](const auto &e) { mOnHoverIn({.point = e.point}); });
    }

    if (mOnHoverOut) {
      dispatcher.registerMouseExitHandler(
          [this](const auto &e) { mOnHoverOut({.point = e.point}); });
    }
  };

  mChild.observe(observeChild);
  observeChild();
}

} // namespace qui
