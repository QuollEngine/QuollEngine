#pragma once

#include "NativeResourceMetrics.h"

namespace liquid::rhi {

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
  void addDrawCall(size_t primitiveCount);

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
  inline uint32_t getDrawCallsCount() const { return mDrawCallsCount; }

  /**
   * @brief Get number of drawn primitives
   *
   * @return Number of drawn primitives
   */
  inline size_t getDrawnPrimitivesCount() const {
    return mDrawnPrimitivesCount;
  }

  /**
   * @brief Get command calls count
   *
   * @return Number of command calls
   */
  inline uint32_t getCommandCallsCount() const { return mCommandCallsCount; }

  /**
   * @brief Get resource metrics
   *
   * @return Resource metrics
   */
  inline const NativeResourceMetrics *getResourceMetrics() const {
    return mResourceMetrics;
  }

private:
  uint32_t mDrawCallsCount = 0;
  size_t mDrawnPrimitivesCount = 0;
  uint32_t mCommandCallsCount = 0;

  NativeResourceMetrics *mResourceMetrics;
};

} // namespace liquid::rhi
