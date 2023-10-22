#pragma once

#include "quoll/rhi/NativeResourceMetrics.h"

namespace quoll::rhi {

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
  usize getTotalBufferSize() const override;

  /**
   * @brief Get number of buffers
   *
   * @return Number of buffers
   */
  usize getBuffersCount() const override;

  /**
   * @brief Get number of textures
   *
   * @return Number of textures
   */
  usize getTexturesCount() const override;

  /**
   * @brief Get number of descriptors
   *
   * @return Number of descriptors
   */
  usize getDescriptorsCount() const override;
};

} // namespace quoll::rhi
