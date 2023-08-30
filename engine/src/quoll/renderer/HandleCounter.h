#pragma once

namespace quoll {

/**
 * @brief Handle counter
 *
 * @tparam THandle Handle type
 * @tparam TStart Starting index
 */
template <class THandle, uint32_t TStart = 1> class HandleCounter {
public:
  /**
   * @brief Create handle
   *
   * @return New handle
   */
  THandle create() {
    uint32_t handle = mLastHandle++;
    return static_cast<THandle>(handle);
  }

private:
  uint32_t mLastHandle = TStart;
};

} // namespace quoll
