#pragma once

#include "../base/NativeRenderCommandListInterface.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDescriptorManager.h"

namespace liquid::experimental {

class VulkanCommandBuffer : public NativeRenderCommandListInterface {
public:
  /**
   * @brief Create Vulkan command buffer
   *
   * @param commandBuffer Command buffer
   * @param registry Resource registry
   * @param descriptorManager Vulkan descriptor manager
   */
  VulkanCommandBuffer(VkCommandBuffer commandBuffer,
                      const VulkanResourceRegistry &registry,
                      VulkanDescriptorManager &descriptorManager);

  /**
   * @brief Get Vulkan command buffer
   *
   * @return Vulkan command buffer
   */
  inline VkCommandBuffer getVulkanCommandBuffer() const {
    return mCommandBuffer;
  }

  /**
   * @brief Begin render pass
   *
   * @param renderPass Render pass
   * @param framebuffer Framebuffer
   * @param renderAreaOffset Render area offset
   * @param renderAreaSize Render area size
   * @param clearValues Clear values
   */
  void beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer,
                       const glm::ivec2 &renderAreaOffset,
                       const glm::uvec2 &renderAreaSize,
                       const std::vector<VkClearValue> &clearValues) override;

  /**
   * @brief End render pass
   */
  void endRenderPass() override;

  /**
   * @brief Bind pipeline
   *
   * @param pipeline Pipeline
   */
  void bindPipeline(const SharedPtr<Pipeline> &pipeline) override;

  /**
   * @brief Bind descriptor
   *
   * @param pipeline Pipeline
   * @param firstSet First set
   * @param descriptor Descriptor
   */
  void bindDescriptor(const SharedPtr<Pipeline> &pipeline, uint32_t firstSet,
                      const Descriptor &descriptor) override;

  /**
   * @brief Bind vertex buffer
   *
   * @param buffer Vertex buffer
   */
  void bindVertexBuffer(BufferHandle buffer) override;

  /**
   * @brief Bind index buffer
   *
   * @param buffer Index buffer
   * @param indexType Index buffer data type
   */
  void bindIndexBuffer(BufferHandle buffer, VkIndexType indexType) override;

  /**
   * @brief Push constants
   *
   * @param pipeline Pipeline
   * @param stageFlags Stage flags
   * @param offset Offset
   * @param size Size
   * @param data Data
   */
  void pushConstants(const SharedPtr<Pipeline> &pipeline,
                     VkShaderStageFlags stageFlags, uint32_t offset,
                     uint32_t size, void *data) override;

  /**
   * @brief Draw
   *
   * @param vertexCount Vertex count
   * @param firstVertex First vertex
   */
  void draw(uint32_t vertexCount, uint32_t firstVertex) override;

  /**
   * @brief Draw indexed
   *
   * @param indexCount Index count
   * @param firstIndex Offset of first index
   * @param vertexOffset Vertex offset
   */
  void drawIndexed(uint32_t indexCount, uint32_t firstIndex,
                   int32_t vertexOffset) override;

  /**
   * @brief Set viewport
   *
   * @param offset Viewport offset
   * @param size Viewport size
   * @param depthRange Viewport depth range
   */
  void setViewport(const glm::vec2 &offset, const glm::vec2 &size,
                   const glm::vec2 &depthRange) override;

  /**
   * @brief Set scissor
   *
   * @param offset Scissor offset
   * @param size Scissor size
   */
  void setScissor(const glm::ivec2 &offset, const glm::uvec2 &size) override;

private:
  VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;

  const VulkanResourceRegistry &mRegistry;
  VulkanDescriptorManager &mDescriptorManager;
};

} // namespace liquid::experimental
