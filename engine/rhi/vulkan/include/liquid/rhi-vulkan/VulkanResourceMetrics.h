#pragma once

#include "liquid/rhi/NativeResourceMetrics.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDescriptorManager.h"

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
   * @param descriptorManager Descriptor manager
   */
  VulkanResourceMetrics(VulkanResourceRegistry &registry,
                        VulkanDescriptorManager &descriptorManager);

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
   * @brief Get size of all the textures
   *
   * @return Total texture size
   */
  size_t getTotalTextureSize() const override;

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
  VulkanDescriptorManager &mDescriptorManager;
};

} // namespace liquid::rhi
