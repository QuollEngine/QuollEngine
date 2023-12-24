#pragma once

namespace quoll::rhi {

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
  virtual usize getTotalBufferSize() const = 0;

  /**
   * @brief Get number of buffers
   *
   * @return Number of buffers
   */
  virtual usize getBuffersCount() const = 0;

  /**
   * @brief Get number of textures
   *
   * @return Number of textures
   */
  virtual usize getTexturesCount() const = 0;

  /**
   * @brief Get number of descriptors
   *
   * @return Number of descriptors
   */
  virtual usize getDescriptorsCount() const = 0;
};

} // namespace quoll::rhi
