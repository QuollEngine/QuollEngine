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

  /**
   * @brief Get size of all the textures
   *
   * @return Total texture size
   */
  size_t getTotalTextureSize() const;

  /**
   * @brief Get number of textures
   *
   * @return Number of textures
   */
  size_t getTexturesCount() const;

private:
  VulkanResourceRegistry &mRegistry;
};

} // namespace liquid::rhi
