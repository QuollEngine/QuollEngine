#pragma once

#include "NativeResourceMetrics.h"

namespace quoll::rhi {

/**
 * @brief Device statistics collector
 *
 * Stores information about how many
 * draw calls etc are done by the device
 */
class DeviceStats {
public:
  /**
   * @brief Create device stats
   *
   * @param resourceMetrics Resource metrics
   */
  DeviceStats(NativeResourceMetrics *resourceMetrics);

  /**
   * @brief Add draw call
   *
   * @param primitiveCount Number of primitives
   */
  void addDrawCall(usize primitiveCount);

  /**
   * @brief Resets calls
   */
  void resetCalls();

  /**
   * @brief Add command call
   */
  void addCommandCall();

  /**
   * @brief Get number of draw calls
   *
   * @return Number of draw calls
   */
  inline u32 getDrawCallsCount() const { return mDrawCallsCount; }

  /**
   * @brief Get number of drawn primitives
   *
   * @return Number of drawn primitives
   */
  inline usize getDrawnPrimitivesCount() const { return mDrawnPrimitivesCount; }

  /**
   * @brief Get command calls count
   *
   * @return Number of command calls
   */
  inline u32 getCommandCallsCount() const { return mCommandCallsCount; }

  /**
   * @brief Get resource metrics
   *
   * @return Resource metrics
   */
  inline const NativeResourceMetrics *getResourceMetrics() const {
    return mResourceMetrics;
  }

private:
  u32 mDrawCallsCount = 0;
  usize mDrawnPrimitivesCount = 0;
  u32 mCommandCallsCount = 0;

  NativeResourceMetrics *mResourceMetrics;
};

} // namespace quoll::rhi
