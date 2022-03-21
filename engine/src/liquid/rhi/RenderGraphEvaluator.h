#pragma once

#include "PipelineDescription.h"
#include "RenderPassDescription.h"
#include "ResourceRegistry.h"
#include "RenderGraph.h"
#include "RenderCommandList.h"

namespace liquid::rhi {

class RenderGraphEvaluator {
  struct RenderPassAttachmentInfo {
    std::vector<TextureHandle> framebufferAttachments;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t layers = 0;
    RenderPassAttachmentDescription attachment;
  };

public:
  /**
   * @brief Create render graph evaluator
   *
   * @param registry Resource registry
   */
  RenderGraphEvaluator(ResourceRegistry &registry);

  /**
   * @brief Build passes
   *
   * @param sorted Topologically sorted passes
   * @param graph Render graph
   * @param swapchainRecreated Rebuild swapchain related passes
   * @param numSwapchainImages Number of swapchain images
   * @param extent Swapchain extent
   */
  void build(std::vector<size_t> &sorted, RenderGraph &graph,
             bool swapchainRecreated, uint32_t numSwapchainImages,
             const glm::uvec2 &extent);

  /**
   * @brief Execute render graph
   *
   * @param commandList Command list
   * @param passes Topologically sorted passes
   * @param graph Render graph
   * @param imageIdx Swapchain image index
   */
  void execute(RenderCommandList &commandList,
               const std::vector<size_t> &passes, RenderGraph &graph,
               uint32_t imageIdx);

  /**
   * @brief Get resource registry
   *
   * @return Resouce registry
   */
  inline ResourceRegistry &getRegistry() { return mRegistry; }

private:
  /**
   * @brief Build render pass resources
   *
   * @param index Render pass index
   * @param graph Render graph
   * @param force Force build even if the pass resources exist
   * @param numSwapchainImages Number of swapchain images
   * @param extent Swapchain extent
   */
  void buildPass(size_t index, RenderGraph &graph, bool force,
                 uint32_t numSwapchainImages, const glm::uvec2 &extent);

  /**
   * @brief Create swapchain attachment
   *
   * @param attachment Attachment description
   * @param numSwapchainAttachments Number of swapchain attachments
   * @param extent Swapchain extent
   * @return Attachment info
   */
  RenderPassAttachmentInfo
  createSwapchainAttachment(const AttachmentData &attachment,
                            uint32_t numSwapchainAttachments,
                            const glm::uvec2 &extent);

  /**
   * @brief Create color attachment
   *
   * @param attachment Attachment description
   * @param texture Texture
   * @param extent Swapchain extent
   * @return Attachment info
   */
  RenderPassAttachmentInfo
  createColorAttachment(const AttachmentData &attachment, TextureHandle texture,
                        const glm::uvec2 &extent);

  /**
   * @brief Create depth attachment
   *
   * @param attachment Attachment description
   * @param texture Texture
   * @param extent Swapchain extent
   * @return Attachment info
   */
  RenderPassAttachmentInfo
  createDepthAttachment(const AttachmentData &attachment, TextureHandle texture,
                        const glm::uvec2 &extent);

  /**
   * @brief Check if pass is has swapchain relative resources
   *
   * @param pass Render pass
   * @retval true Has swapchain resources
   * @retval false Does not have swapchain relative resources
   */
  bool hasSwapchainRelativeResources(RenderGraphPass &pass);

private:
  ResourceRegistry &mRegistry;
};

} // namespace liquid::rhi
