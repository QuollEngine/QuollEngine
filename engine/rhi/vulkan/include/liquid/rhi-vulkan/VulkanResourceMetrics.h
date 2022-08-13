#pragma once

#include "liquid/rhi/NativeResourceMetrics.h"
#include "VulkanResourceRegistry.h"

namespace liquid::rhi {

/**
 * @brief Vulkan resource metrics
 */
class VulkanResourceMetrics : public NativeResourceMetrics {
public:
  /**
   * @brief Create Vulkan resource metrics
   *
   * @param registry Vulkan resource registry
   */
  VulkanResourceMetrics(VulkanResourceRegistry &registry);

  /**
   * @brief Get size of all the buffers
   *
   * @return Total buffer size
   */
  size_t getTotalBufferSize() const;

  /**
   * @brief Get number of buffers
   *
   * @return Number of buffers
   */
  size_t getBuffersCount() const;

private:
  VulkanResourceRegistry &mRegistry;
};

} // namespace liquid::rhi
