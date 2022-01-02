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
  std::vector<RenderGraphPassBase *> build(RenderGraph &graph);

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
               const std::vector<RenderGraphPassBase *> &passes,
               RenderGraph &graph, uint32_t imageIdx);

private:
  /**
   * @brief Build render pass resources
   *
   * @param pass Render pass
   * @param graph Render graph
   * @param force Force build even if the pass resources exist
   */
  void buildPass(RenderGraphPassBase *pass, RenderGraph &graph, bool force);

  /**
   * @brief Create swapchain attachment
   *
   * @param attachment Attachment description
   * @param index Attachment index
   * @return Attachment info
   */
  VulkanAttachmentInfo
  createSwapchainAttachment(const RenderPassAttachment &attachment,
                            uint32_t index);

  /**
   * @brief Create color attachment
   *
   * @param attachment Attachment description
   * @param texture Texture
   * @param index Attachment index
   * @return Attachment info
   */
  VulkanAttachmentInfo
  createColorAttachment(const RenderPassAttachment &attachment,
                        const SharedPtr<Texture> &texture, uint32_t index);

  /**
   * @brief Create depth attachment
   *
   * @param attachment Attachment description
   * @param texture Texture
   * @param index Attachment index
   * @return Attachment info
   */
  VulkanAttachmentInfo
  createDepthAttachment(const RenderPassAttachment &attachment,
                        const SharedPtr<Texture> &texture, uint32_t index);

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

  /**
   * @brief Create texture
   *
   * @param resourceId Resource Id
   * @param data Attachment data
   * @return Texture
   */
  SharedPtr<Texture> createTexture(GraphResourceId resourceId,
                                   const AttachmentData &data);

private:
  VulkanContext &vulkanInstance;
  VulkanSwapchain &swapchain;
  ResourceAllocator *resourceAllocator;
};

} // namespace liquid
