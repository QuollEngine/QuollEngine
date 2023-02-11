#pragma once

#include "liquid/rhi/NativeResourceMetrics.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDescriptorPool.h"

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
   * @param descriptorPool Descriptor pool
   */
  VulkanResourceMetrics(VulkanResourceRegistry &registry,
                        VulkanDescriptorPool &descriptorPool);

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
   * @return Descriptors
   */
  size_t getDescriptorsCount() const override;

private:
  VulkanResourceRegistry &mRegistry;
  VulkanDescriptorPool &mDescriptorPool;
};

} // namespace liquid::rhi
