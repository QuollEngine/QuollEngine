#pragma once

#include "quoll/rhi/DeviceStats.h"
#include "quoll/rhi/NativeRenderCommandListInterface.h"

#include "VulkanResourceRegistry.h"
#include "VulkanDescriptorPool.h"

namespace quoll::rhi {

/**
 * @brief Vulkan command buffer
 */
class VulkanCommandBuffer : public NativeRenderCommandListInterface {
public:
  /**
   * @brief Create Vulkan command buffer
   *
   * @param commandBuffer Command buffer
   * @param registry Resource registry
   * @param descriptorPool Descriptor pool
   * @param stats Device stats
   */
  VulkanCommandBuffer(VkCommandBuffer commandBuffer,
                      const VulkanResourceRegistry &registry,
                      const VulkanDescriptorPool &descriptorPool,
                      DeviceStats &stats);

  /**
   * @brief Destructor
   */
  ~VulkanCommandBuffer() = default;

  VulkanCommandBuffer(const VulkanCommandBuffer &) = delete;
  VulkanCommandBuffer &operator=(const VulkanCommandBuffer &) = delete;
  VulkanCommandBuffer(VulkanCommandBuffer &&) = delete;
  VulkanCommandBuffer &operator=(VulkanCommandBuffer &&) = delete;

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
   */
  void beginRenderPass(rhi::RenderPassHandle renderPass,
                       FramebufferHandle framebuffer,
                       const glm::ivec2 &renderAreaOffset,
                       const glm::uvec2 &renderAreaSize) override;

  /**
   * @brief End render pass
   */
  void endRenderPass() override;

  /**
   * @brief Bind pipeline
   *
   * @param pipeline Pipeline
   */
  void bindPipeline(PipelineHandle pipeline) override;

  /**
   * @brief Bind descriptor
   *
   * @param pipeline Pipeline
   * @param firstSet First set
   * @param descriptor Descriptor
   * @param dynamicOffsets Dynamic offsets
   */
  void bindDescriptor(PipelineHandle pipeline, u32 firstSet,
                      const Descriptor &descriptor,
                      std::span<u32> dynamicOffsets) override;

  /**
   * @brief Bind vertex buffers
   *
   * @param buffers Vertex buffers
   * @param offsets Vertex buffer binding offsets
   */
  void bindVertexBuffers(const std::span<const BufferHandle> buffers,
                         const std::span<const u64> offsets) override;

  /**
   * @brief Bind index buffer
   *
   * @param buffer Index buffer
   * @param indexType Index buffer data type
   */
  void bindIndexBuffer(BufferHandle buffer, IndexType indexType) override;

  /**
   * @brief Push constants
   *
   * @param pipeline Pipeline
   * @param shaderStage Shader stage
   * @param offset Offset
   * @param size Size
   * @param data Data
   */
  void pushConstants(PipelineHandle pipeline, ShaderStage shaderStage,
                     u32 offset, u32 size, void *data) override;

  /**
   * @brief Draw
   *
   * @param vertexCount Vertex count
   * @param firstVertex First vertex
   * @param instanceCount Instance count
   * @param firstInstance First instance
   */
  void draw(u32 vertexCount, u32 firstVertex, u32 instanceCount,
            u32 firstInstance) override;

  /**
   * @brief Draw indexed
   *
   * @param indexCount Index count
   * @param firstIndex Offset of first index
   * @param vertexOffset Vertex offset
   * @param instanceCount Instance count
   * @param firstInstance First instance
   */
  void drawIndexed(u32 indexCount, u32 firstIndex, i32 vertexOffset,
                   u32 instanceCount, u32 firstInstance) override;

  /**
   * @brief Dispatch compute work
   *
   * @param groupCountX Number of groups to dispatch in X direction
   * @param groupCountY Number of groups to dispatch in Y direction
   * @param groupCountZ Number of groups to dispatch in Z direction
   */
  void dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) override;

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

  /**
   * @brief Pipeline barrier
   *
   * @param memoryBarriers Memory barriers
   * @param imageBarriers Image barriers
   * @param bufferBarriers Buffer barriers
   */
  void pipelineBarrier(std::span<MemoryBarrier> memoryBarriers,
                       std::span<ImageBarrier> imageBarriers,
                       std::span<BufferBarrier> bufferBarriers) override;

  /**
   * @brief Copy texture to buffer
   *
   * @param srcTexture Source texture
   * @param dstBuffer Destination buffer
   * @param copyRegions Copy regions
   */
  void copyTextureToBuffer(TextureHandle srcTexture, BufferHandle dstBuffer,
                           std::span<CopyRegion> copyRegions) override;

  /**
   * @brief Copy buffer to texture
   *
   * @param srcBuffer Source buffer
   * @param dstTexture Destination texture
   * @param copyRegions Copy regions
   */
  void copyBufferToTexture(BufferHandle srcBuffer, TextureHandle dstTexture,
                           std::span<CopyRegion> copyRegions) override;

  /**
   * @brief Blit texture
   *
   * @param source Source texture
   * @param destination Destination texture
   * @param regions Blit regions
   * @param filter Filter
   */
  void blitTexture(TextureHandle source, TextureHandle destination,
                   std::span<BlitRegion> regions, Filter filter) override;

private:
  VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;

  const VulkanResourceRegistry &mRegistry;
  const VulkanDescriptorPool &mDescriptorPool;
  DeviceStats &mStats;
};

} // namespace quoll::rhi
