#pragma once

#include "RenderGraphPassResult.h"
#include "liquid/rhi/RenderHandle.h"
#include <vulkan/vulkan.hpp>

namespace liquid {

class RenderGraphPassResult {
public:
  RenderGraphPassResult(RenderPassHandle renderPass,
                        const std::vector<FramebufferHandle> &framebuffers,
                        const std::vector<VkClearValue> &clearValues,
                        uint32_t width, uint32_t height, uint32_t layers);

  /**
   * @brief Get framebuffers
   *
   * @return List of framebuffers
   */
  inline const std::vector<FramebufferHandle> &getFramebuffers() const {
    return framebuffers;
  };

  /**
   * @brief Get render pass
   *
   * @return Render pass handle
   */
  inline RenderPassHandle getRenderPass() const { return renderPass; }

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
  RenderPassHandle renderPass;
  std::vector<FramebufferHandle> framebuffers;
  std::vector<VkClearValue> clearValues;
  bool swapchainRelative = false;

  glm::uvec2 extent;

  uint32_t layers;
};

} // namespace liquid
