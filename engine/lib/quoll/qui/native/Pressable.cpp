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
  mEventHolder = context.eventManager->createEventHolder();

  auto observeChild = [this, &context]() { mChild().build(context); };

  mChild.observe(observeChild);
  observeChild();

  if (mOnPress) {
    mEventHolder.registerMouseClickHandler(
        [this](const MouseEvent &mouseEvent) {
          HitTestResult hitResult{};

          if (getView()->hitTest(mouseEvent.point, hitResult)) {
            mOnPress({.point = mouseEvent.point});
          }
        });
  }

  if (mOnPressDown) {
    mEventHolder.registerMouseDownHandler([this](const MouseEvent &mouseEvent) {
      HitTestResult hitResult{};

      if (getView()->hitTest(mouseEvent.point, hitResult)) {
        mOnPressDown({.point = mouseEvent.point});
      }
    });
  }

  if (mOnPressUp) {
    mEventHolder.registerMouseUpHandler([this](const MouseEvent &mouseEvent) {
      HitTestResult hitResult{};
      if (getView()->hitTest(mouseEvent.point, hitResult)) {
        mOnPressUp({.point = mouseEvent.point});
      }
    });
  }

  if (mOnHoverIn || mOnHoverOut) {
    mEventHolder.registerMouseMoveHandler([this](const MouseEvent &mouseEvent) {
      HitTestResult hitResult{};

      const bool hitTest = getView()->hitTest(mouseEvent.point, hitResult);

      if (!mHovered && hitTest) {
        mHovered = true;
        if (mOnHoverIn) {
          mOnHoverIn({.point = mouseEvent.point});
        }
      } else if (mHovered && !hitTest) {
        mHovered = false;
        if (mOnHoverOut) {
          mOnHoverOut({.point = mouseEvent.point});
        }
      }
    });
  }
}

} // namespace qui
