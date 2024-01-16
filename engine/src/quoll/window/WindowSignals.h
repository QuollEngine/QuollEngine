#pragma once

#include "quoll/signals/Signal.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"

namespace quoll {

class WindowSignals {
public:
  inline Signal<KeyboardEvent> &onKeyPress() { return mOnKeyPressSignal; }

  inline Signal<KeyboardEvent> &onKeyRelease() { return mOnKeyReleaseSignal; }

  inline Signal<MouseButtonEvent> &onMousePress() {
    return mOnMousePressSignal;
  }

  inline Signal<MouseButtonEvent> &onMouseRelease() {
    return mOnMouseReleaseSignal;
  }

  inline Signal<MouseCursorEvent> &onMouseMove() { return mOnMouseMoveSignal; }

  inline Signal<MouseScrollEvent> &onMouseScroll() {
    return mOnMouseScrollSignal;
  }

  inline Signal<u32, u32> &onFramebufferResize() {
    return mOnFramebufferResizeSignal;
  }

  inline Signal<bool> &onFocus() { return mOnFocusSignal; }

private:
  Signal<KeyboardEvent> mOnKeyPressSignal;
  Signal<KeyboardEvent> mOnKeyReleaseSignal;

  Signal<MouseButtonEvent> mOnMousePressSignal;
  Signal<MouseButtonEvent> mOnMouseReleaseSignal;

  Signal<MouseCursorEvent> mOnMouseMoveSignal;

  Signal<MouseScrollEvent> mOnMouseScrollSignal;

  Signal<u32, u32> mOnFramebufferResizeSignal;

  Signal<bool> mOnFocusSignal;
};

} // namespace quoll
