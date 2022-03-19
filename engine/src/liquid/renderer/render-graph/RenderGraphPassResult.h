#pragma once

#include "RenderGraphPassResult.h"
#include "liquid/rhi/RenderHandle.h"
#include <vulkan/vulkan.hpp>

namespace liquid {

class RenderGraphPassResult {
public:
  RenderGraphPassResult(rhi::RenderPassHandle renderPass,
                        const std::vector<rhi::FramebufferHandle> &framebuffers,
                        const std::vector<VkClearValue> &clearValues,
                        uint32_t width, uint32_t height, uint32_t layers);

  /**
   * @brief Get framebuffers
   *
   * @return List of framebuffers
   */
  inline const std::vector<rhi::FramebufferHandle> &getFramebuffers() const {
    return mFramebuffers;
  };

  /**
   * @brief Get render pass
   *
   * @return Render pass handle
   */
  inline rhi::RenderPassHandle getRenderPass() const { return mRenderPass; }

  /**
   * @brief Get clear values
   *
   * @return List of clear values
   */
  inline const std::vector<VkClearValue> &getClearValues() const {
    return mClearValues;
  }

  /**
   * @brief Get extent
   *
   * @return Extent
   */
  inline glm::uvec2 getExtent() const { return mExtent; }

private:
  rhi::RenderPassHandle mRenderPass;
  std::vector<rhi::FramebufferHandle> mFramebuffers;
  std::vector<VkClearValue> mClearValues;
  bool mSwapchainRelative = false;

  glm::uvec2 mExtent;

  uint32_t mLayers;
};

} // namespace liquid
