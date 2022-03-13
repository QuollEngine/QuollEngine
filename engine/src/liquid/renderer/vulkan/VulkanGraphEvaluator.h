#pragma once

#include "liquid/renderer/render-graph/RenderGraph.h"
#include "liquid/rhi/RenderCommandList.h"
#include "liquid/renderer/Pipeline.h"

#include "liquid/rhi/vulkan/VulkanSwapchain.h"
#include "liquid/renderer/render-graph/RenderGraphPipelineDescriptor.h"
#include "liquid/rhi/vulkan/VulkanRenderDevice.h"

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
   * @param device Vulkan device
   * @param swapchain Swapchain
   * @param registry Resource registry
   * @param realRegistry Real registry of resources
   */
  VulkanGraphEvaluator(
      experimental::VulkanRenderDevice *device,
      experimental::ResourceRegistry &registry,
      const experimental::VulkanResourceRegistry &realRegistry);

  /**
   * @brief Compile render graph
   *
   * @param graph Render graph
   * @param swapchainRecreated Compile swapchain related passes
   * @param extent Swapchain extent
   * @return Topologically sorted list of passes
   */
  std::vector<RenderGraphPassBase *> compile(RenderGraph &graph,
                                             bool swapchainRecreated,
                                             const glm::uvec2 &extent);

  /**
   * @brief Build passes
   *
   * @param compiled Compiled passes
   * @param graph Render graph
   * @param swapchainRecreated Rebuild swapchain related passes
   * @param numSwapchainImages Number of swapchain images
   * @param extent Swapchain extent
   */
  void build(std::vector<RenderGraphPassBase *> &compiled, RenderGraph &graph,
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
               const std::vector<RenderGraphPassBase *> &passes,
               RenderGraph &graph, uint32_t imageIdx);

private:
  /**
   * @brief Build render pass resources
   *
   * @param pass Render pass
   * @param graph Render graph
   * @param force Force build even if the pass resources exist
   * @param numSwapchainImages Number of swapchain images
   * @param extent Swapchain extent
   */
  void buildPass(RenderGraphPassBase *pass, RenderGraph &graph, bool force,
                 uint32_t numSwapchainImages, const glm::uvec2 &extent);

  /**
   * @brief Create swapchain attachment
   *
   * @param attachment Attachment description
   * @param index Attachment index
   * @param numSwapchainAttachments Number of swapchain attachments
   * @param extent Swapchain extent
   * @return Attachment info
   */
  VulkanAttachmentInfo
  createSwapchainAttachment(const RenderPassAttachment &attachment,
                            uint32_t index, uint32_t numSwapchainAttachments,
                            const glm::uvec2 &extent);

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
                        TextureHandle texture, uint32_t index);

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
                        TextureHandle texture, uint32_t index);

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
   * @param data Attachment data
   * @param extent Swapchain extent
   * @return Texture
   */
  TextureHandle createTexture(const AttachmentData &data,
                              const glm::uvec2 &extent);

private:
  experimental::VulkanRenderDevice *device;
  experimental::ResourceRegistry &registry;
  const experimental::VulkanResourceRegistry &realRegistry;
};

} // namespace liquid
