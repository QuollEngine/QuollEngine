#pragma once

#include "liquid/rhi/PipelineDescription.h"
#include "liquid/rhi/RenderPassDescription.h"
#include "liquid/rhi/RenderCommandList.h"
#include "liquid/rhi/RenderDevice.h"

#include "RenderGraph.h"

namespace liquid {

/**
 * @brief Render graph evaluator
 */
class RenderGraphEvaluator {
  struct RenderPassAttachmentInfo {
    std::vector<rhi::TextureHandle> framebufferAttachments;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t layers = 0;
    rhi::RenderPassAttachmentDescription attachment;
  };

public:
  /**
   * @brief Create render graph evaluator
   *
   * @param device Render device
   */
  RenderGraphEvaluator(rhi::RenderDevice *device);

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
   * @param frameIndex Frame index
   */
  void execute(rhi::RenderCommandList &commandList, RenderGraph &graph,
               uint32_t frameIndex);

private:
  /**
   * @brief Build graphics pass resources
   *
   * @param index Render pass index
   * @param graph Render graph
   * @param force Force build even if the pass resources exist
   */
  void buildGraphicsPass(size_t index, RenderGraph &graph, bool force);

  /**
   * @brief Build compute pass resources
   *
   * @param index Render pass index
   * @param graph Render graph
   * @param force Force build even if the pass resources exist
   */
  void buildComputePass(size_t index, RenderGraph &graph, bool force);

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
  rhi::RenderDevice *mDevice = nullptr;
};

} // namespace liquid
