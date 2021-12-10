#pragma once

#include "renderer/Pipeline.h"

#include <vulkan/vulkan.hpp>

namespace liquid {

class VulkanPipeline : public Pipeline {
public:
  /**
   * Constructor to set device and pipeline
   *
   * @param device Vulkan device
   * @param pipeline Vulkan pipeline handle
   */
  VulkanPipeline(VkDevice device, VkPipeline pipeline);

  /**
   * @brief Destructor
   *
   * Destroys Vulkan pipeline
   */
  ~VulkanPipeline();

  VulkanPipeline(const VulkanPipeline &) = delete;
  VulkanPipeline(VulkanPipeline &&) = delete;
  VulkanPipeline &operator=(const VulkanPipeline &) = delete;
  VulkanPipeline &operator=(VulkanPipeline &&) = delete;

  /**
   * @brief Get Vulkan pipeline
   *
   * @return Vulkan pipeline
   */
  inline VkPipeline getPipeline() const { return pipeline; }

private:
  VkDevice device = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;
};

} // namespace liquid
