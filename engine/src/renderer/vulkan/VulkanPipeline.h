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
   * @param pipelineLayout Vulkan pipeline layout handle
   * @param descriptorSetLayout Descriptor set layout handles
   */
  VulkanPipeline(
      VkDevice device, VkPipeline pipeline, VkPipelineLayout pipelineLayout,
      const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts);

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

  /**
   * @brief Get Vulkan pipeline layout
   *
   * @return Vulkan pipeline layout
   */
  inline VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

  /**
   * @brief Get descriptor layout at index
   *
   * @param index Descriptor layout index
   * @return Descriptor layout
   */
  inline const VkDescriptorSetLayout getDescriptorLayout(uint32_t index) const {
    return descriptorSetLayouts.at(index);
  }

private:
  VkDevice device = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
};

} // namespace liquid
