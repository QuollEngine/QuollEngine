#pragma once

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

private:
  uint32_t mDrawCallsCount = 0;
  size_t mDrawnPrimitivesCount = 0;
  uint32_t mCommandCallsCount = 0;
};

} // namespace liquid::rhi
