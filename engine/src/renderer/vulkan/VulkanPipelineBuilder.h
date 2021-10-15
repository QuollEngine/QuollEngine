#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanDescriptorManager.h"
#include "VulkanPipeline.h"
#include "renderer/CullMode.h"

namespace liquid {

class VulkanPipelineBuilder {
public:
  /**
   * @brief Creates pipeline builder
   *
   * @param device Vulkan device
   * @param descriptorManager Descriptor manager
   */
  VulkanPipelineBuilder(VkDevice device,
                        VulkanDescriptorManager *descriptorManager);

  /**
   * @brief Destroys pipeline builder
   *
   * Destroys pipeline layout
   */
  ~VulkanPipelineBuilder();

  VulkanPipelineBuilder(const VulkanPipelineBuilder &rhs) = delete;
  VulkanPipelineBuilder(VulkanPipelineBuilder &&rhs) = delete;
  VulkanPipelineBuilder &operator=(const VulkanPipelineBuilder &rhs) = delete;
  VulkanPipelineBuilder &operator=(VulkanPipelineBuilder &&rhs) = delete;

  /**
   * @brief Creates pipeline from pipeline layout and shader stages
   *
   * @param stages Shader stages
   * @param polygonMode Polygon mode
   * @param renderPass Vulkan render pass
   * @param subpass Vulkan render pass subpass index
   * @param cullMode Cull mode
   * @return Vulkan pipeline
   */
  VulkanPipeline build(VkPipelineShaderStageCreateInfo *stages,
                       VkPolygonMode polygonMode, VkRenderPass renderPass,
                       uint32_t subpass, const CullMode &cullMode) const;

  /**
   * @brief Gets device
   *
   * @return Vulkan device
   */
  inline VkDevice getDevice() { return device; }

  /**
   * @brief Gets pipeline layout
   *
   * @return Vulkan pipeline layout
   */
  inline VkPipelineLayout getPipelineLayout() { return pipelineLayout; }

private:
  /**
   * @brief Create pipeline layout
   *
   * @param descriptorManager Descriptor manager
   */
  void createPipelineLayout(VulkanDescriptorManager *descriptorManager);

private:
  /**
   * @brief Get cull mode understandable to Vulkan
   *
   * @param cullMode Cull mode
   * @return Vulkan cull mode flags
   */
  static VkCullModeFlags getVulkanCullMode(const CullMode &cullMode);

private:
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkDevice device = nullptr;
};

} // namespace liquid
