#pragma once

namespace quoll {

template <class THandle, u32 TStart = 1> class HandleCounter {
public:
  THandle create() {
    u32 handle = mLastHandle++;
    return static_cast<THandle>(handle);
  }

private:
  u32 mLastHandle = TStart;
};

} // namespace quoll
