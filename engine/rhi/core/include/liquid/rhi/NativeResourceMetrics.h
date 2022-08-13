#pragma once

namespace liquid::rhi {

/**
 * @brief Interface for native resource metrics
 */
class NativeResourceMetrics {
public:
  /**
   * @brief Get size of all the buffers
   *
   * @return Total buffer size
   */
  virtual size_t getTotalBufferSize() const = 0;

  /**
   * @brief Get number of buffers
   *
   * @return Number of buffers
   */
  virtual size_t getBuffersCount() const = 0;
};

} // namespace liquid::rhi