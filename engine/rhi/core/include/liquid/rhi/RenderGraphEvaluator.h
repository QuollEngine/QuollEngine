#pragma once

#include "PipelineDescription.h"
#include "RenderPassDescription.h"
#include "ResourceRegistry.h"
#include "RenderGraph.h"
#include "RenderCommandList.h"
#include "RenderDevice.h"

namespace liquid::rhi {

/**
 * @brief Render graph evaluator
 */
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
   * @param device Render device
   */
  RenderGraphEvaluator(ResourceRegistry &registry, RenderDevice *device);

  /**
   * @brief Build render graph
   *
   * @param graph Render graph
   */
  void build(RenderGraph &graph);

  /**
   * @brief Execute render graph
   *
   * @param commandList Command list
   * @param graph Render graph
   */
  void execute(RenderCommandList &commandList, RenderGraph &graph);

private:
  /**
   * @brief Build render pass resources
   *
   * @param index Render pass index
   * @param graph Render graph
   * @param force Force build even if the pass resources exist
   */
  void buildPass(size_t index, RenderGraph &graph, bool force);

  /**
   * @brief Create attachment
   *
   * @param attachment Attachment description
   * @param renderTarget Render target data
   * @param extent Swapchain extent
   * @return Attachment info
   */
  RenderPassAttachmentInfo createAttachment(const AttachmentData &attachment,
                                            RenderTargetData &renderTarget,
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

  RenderDevice *mDevice = nullptr;
};

} // namespace liquid::rhi
