#pragma once

#include "quoll/signals/Signal.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"

namespace quoll {

/**
 * @brief Window signals
 */
class WindowSignals {
public:
  /**
   * @brief Get key down signal
   *
   * @return Key down signal
   */
  inline Signal<KeyboardEvent> &onKeyPress() { return mOnKeyPressSignal; }

  /**
   * @brief Get key up signal
   *
   * @return Key up signal
   */
  inline Signal<KeyboardEvent> &onKeyRelease() { return mOnKeyReleaseSignal; }

  /**
   * @brief Get mouse press signal
   *
   * @return Mouse press signal
   */
  inline Signal<MouseButtonEvent> &onMousePress() {
    return mOnMousePressSignal;
  }

  /**
   * @brief Get mouse release signal
   *
   * @return Mouse release signal
   */
  inline Signal<MouseButtonEvent> &onMouseRelease() {
    return mOnMouseReleaseSignal;
  }

  /**
   * @brief Get mouse move signal
   *
   * @return Mouse move signal
   */
  inline Signal<MouseCursorEvent> &onMouseMove() { return mOnMouseMoveSignal; }

  /**
   * @brief Get mouse scroll signal
   *
   * @return Mouse scroll signal
   */
  inline Signal<MouseScrollEvent> &onMouseScroll() {
    return mOnMouseScrollSignal;
  }

  /**
   * @brief Get mouse scroll signal
   *
   * @return Mouse scroll signal
   */
  inline Signal<u32, u32> &onFramebufferResize() {
    return mOnFramebufferResizeSignal;
  }

  /**
   * @brief Get mouse scroll signal
   *
   * @return Mouse scroll signal
   */
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
