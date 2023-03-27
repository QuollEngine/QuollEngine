#pragma once

#include "liquid/rhi/NativeResourceMetrics.h"

namespace liquid::rhi {

/**
 * @brief Mock resource metrics
 */
class MockResourceMetrics : public NativeResourceMetrics {
public:
  /**
   * @brief Get size of all the buffers
   *
   * @return Total buffer size
   */
  size_t getTotalBufferSize() const override;

  /**
   * @brief Get number of buffers
   *
   * @return Number of buffers
   */
  size_t getBuffersCount() const override;

  /**
   * @brief Get number of textures
   *
   * @return Number of textures
   */
  size_t getTexturesCount() const override;

  /**
   * @brief Get number of descriptors
   *
   * @return Number of descriptors
   */
  size_t getDescriptorsCount() const override;
};

} // namespace liquid::rhi
