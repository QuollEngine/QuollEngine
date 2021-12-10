#pragma once

#include "renderer/RenderPass.h"
#include <vulkan/vulkan.hpp>

namespace liquid {

class VulkanRenderPass : public RenderPass {
public:
  VulkanRenderPass(VkDevice device, VkRenderPass renderPass,
                   const std::vector<VkFramebuffer> &framebuffers,
                   const std::vector<VkClearValue> &clearValues, uint32_t width,
                   uint32_t height, uint32_t layers);

  ~VulkanRenderPass();

  VulkanRenderPass(const VulkanRenderPass &) = delete;
  VulkanRenderPass(VulkanRenderPass &&) = delete;
  VulkanRenderPass &operator=(const VulkanRenderPass &) = delete;
  VulkanRenderPass &operator=(VulkanRenderPass &&) = delete;

  /**
   * @brief Get framebuffers
   *
   * @return List of framebuffers
   */
  inline const std::vector<VkFramebuffer> &getFramebuffers() const {
    return framebuffers;
  };

  /**
   * @brief Get render pass
   *
   * @return Render pass handle
   */
  inline VkRenderPass getRenderPass() const { return renderPass; }

  /**
   * @brief Get clear values
   *
   * @return List of clear values
   */
  inline const std::vector<VkClearValue> &getClearValues() const {
    return clearValues;
  }

  /**
   * @brief Get extent
   *
   * @return Extent
   */
  inline glm::uvec2 getExtent() const { return extent; }

private:
  VkDevice device;
  VkRenderPass renderPass;
  std::vector<VkFramebuffer> framebuffers;
  std::vector<VkClearValue> clearValues;
  bool swapchainRelative = false;

  glm::uvec2 extent;

  uint32_t layers;
};

} // namespace liquid
