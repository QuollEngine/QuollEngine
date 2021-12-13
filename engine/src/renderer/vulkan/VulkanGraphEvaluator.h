#pragma once

#include "renderer/render-graph/RenderGraph.h"
#include "renderer/render-graph/RenderGraphPipelineDescriptor.h"
#include "renderer/ResourceAllocator.h"
#include "renderer/RenderCommandList.h"
#include "renderer/Pipeline.h"

#include "VulkanContext.h"
#include "VulkanSwapchain.h"

namespace liquid {

class VulkanGraphEvaluator {
  struct VulkanAttachmentInfo {
    VkAttachmentDescription description{};
    VkAttachmentReference reference{};
    VkClearValue clearValue;
    std::vector<VkImageView> framebufferAttachments;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t layers = 0;
    SharedPtr<Texture> texture = nullptr;
  };

public:
  /**
   * @brief Create graph evaluator
   *
   * @param vulkanInstance Vulkan instance
   * @param swapchain Swapchain
   * @param resourceAllocator Resource allocator
   */
  VulkanGraphEvaluator(VulkanContext &vulkanInstance,
                       VulkanSwapchain &swapchain,
                       ResourceAllocator *resourceAllocator);

  /**
   * @brief Build render graph
   *
   * @param graph Render graph
   * @return Topologically sorted render passes
   */
  std::vector<RenderGraphPassInterface *> build(RenderGraph &graph);

  /**
   * @brief Rebuild swapchain related passes
   *
   * @param graph Render graph
   */
  void rebuildSwapchainRelatedPasses(RenderGraph &graph);

  /**
   * @brief Execute render graph
   *
   * @param commandList Command list
   * @param passes Topologically sorted passes
   * @param graph Render graph
   * @param imageIdx Swapchain image index
   */
  void execute(RenderCommandList &commandList,
               const std::vector<RenderGraphPassInterface *> &passes,
               RenderGraph &graph, uint32_t imageIdx);

private:
  /**
   * @brief Build render pass resources
   *
   * @param pass Render pass
   * @param graph Render graph
   * @param force Force build even if the pass resources exist
   */
  void buildPass(RenderGraphPassInterface *pass, RenderGraph &graph,
                 bool force);

  /**
   * @brief Create swapchain color attachment
   *
   * @param attachment Swapchain attachment description
   * @param index Attachment index
   * @return Attachment info
   */
  VulkanAttachmentInfo createSwapchainColorAttachment(
      const RenderPassSwapchainAttachment &attachment, uint32_t index);

  /**
   * @brief Create swapchain depth attachment
   *
   * @param attachment Swapchain attachment description
   * @param index Attachment index
   * @return Attachment info
   */
  VulkanAttachmentInfo createSwapchainDepthAttachment(
      const RenderPassSwapchainAttachment &attachment, uint32_t index);

  /**
   * @brief Create color attachment
   *
   * @param attachment Attachment description
   * @param index Attachment index
   * @return Attachment info
   */
  VulkanAttachmentInfo
  createColorAttachment(const RenderPassAttachment &attachment, uint32_t index);

  /**
   * @brief Create depth attachment
   *
   * @param attachment Attachment description
   * @param index Attachment index
   * @return Attachment info
   */
  VulkanAttachmentInfo
  createDepthAttachment(const RenderPassAttachment &attachment, uint32_t index);

  /**
   * @brief Create graphics pipeline
   *
   * @param descriptor Pipeline descriptor
   * @param renderPass Render pass
   * @return Pipeline
   */
  const SharedPtr<Pipeline>
  createGraphicsPipeline(const PipelineDescriptor &descriptor,
                         VkRenderPass renderPass);

private:
  VulkanContext &vulkanInstance;
  VulkanSwapchain &swapchain;
  ResourceAllocator *resourceAllocator;
};

} // namespace liquid
