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
  inline Signal<KeyboardEventObject> &getKeyDownSignal() {
    return mKeyDownSignal;
  }

  /**
   * @brief Get key up signal
   *
   * @return Key up signal
   */
  inline Signal<KeyboardEventObject> &getKeyUpSignal() { return mKeyUpSignal; }

private:
  Signal<KeyboardEventObject> mKeyDownSignal;
  Signal<KeyboardEventObject> mKeyUpSignal;
};

} // namespace quoll
