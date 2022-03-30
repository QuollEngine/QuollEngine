#pragma once

namespace liquid::rhi {

class DeviceStats {
public:
  /**
   * @brief Add draw call
   *
   * @brief primitiveCount Number of primitives
   */
  void addDrawCall(size_t primitiveCount);

  /**
   * @brief Resets draw calls
   */
  void resetDrawCalls();

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

private:
  uint32_t mDrawCallsCount = 0;
  size_t mDrawnPrimitivesCount = 0;
};

} // namespace liquid::rhi
