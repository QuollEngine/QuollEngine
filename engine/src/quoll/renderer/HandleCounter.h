#pragma once

namespace quoll {

/**
 * @brief Handle counter
 *
 * @tparam THandle Handle type
 * @tparam TStart Starting index
 */
template <class THandle, u32 TStart = 1> class HandleCounter {
public:
  /**
   * @brief Create handle
   *
   * @return New handle
   */
  THandle create() {
    u32 handle = mLastHandle++;
    return static_cast<THandle>(handle);
  }

private:
  u32 mLastHandle = TStart;
};

} // namespace quoll
