#pragma once

#include "quoll/signals/Signal.h"

namespace quoll::lua {

class ScriptLoop {
public:
  inline Signal<f32> &onUpdate() { return mOnUpdateSignal; }

private:
  Signal<f32> mOnUpdateSignal;
};

} // namespace quoll::lua
