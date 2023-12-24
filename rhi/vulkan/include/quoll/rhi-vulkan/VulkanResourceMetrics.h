#pragma once

#include "quoll/rhi/NativeResourceMetrics.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDescriptorPool.h"

namespace quoll::rhi {

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
   * @return Descriptors
   */
  usize getDescriptorsCount() const override;

private:
  VulkanResourceRegistry &mRegistry;
  VulkanDescriptorPool &mDescriptorPool;
};

} // namespace quoll::rhi
