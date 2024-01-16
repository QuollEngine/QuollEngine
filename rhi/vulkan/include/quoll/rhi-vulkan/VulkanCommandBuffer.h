#pragma once

#include "quoll/rhi/DeviceStats.h"
#include "quoll/rhi/NativeRenderCommandListInterface.h"

#include "VulkanResourceRegistry.h"
#include "VulkanDescriptorPool.h"

namespace quoll::rhi {

class VulkanCommandBuffer : public NativeRenderCommandListInterface {
public:
  VulkanCommandBuffer(VkCommandBuffer commandBuffer,
                      const VulkanResourceRegistry &registry,
                      const VulkanDescriptorPool &descriptorPool,
                      DeviceStats &stats);

  ~VulkanCommandBuffer() = default;

  VulkanCommandBuffer(const VulkanCommandBuffer &) = delete;
  VulkanCommandBuffer &operator=(const VulkanCommandBuffer &) = delete;
  VulkanCommandBuffer(VulkanCommandBuffer &&) = delete;
  VulkanCommandBuffer &operator=(VulkanCommandBuffer &&) = delete;

  inline VkCommandBuffer getVulkanCommandBuffer() const {
    return mCommandBuffer;
  }

  void beginRenderPass(rhi::RenderPassHandle renderPass,
                       FramebufferHandle framebuffer,
                       const glm::ivec2 &renderAreaOffset,
                       const glm::uvec2 &renderAreaSize) override;

  void endRenderPass() override;

  void bindPipeline(PipelineHandle pipeline) override;

  void bindDescriptor(PipelineHandle pipeline, u32 firstSet,
                      const Descriptor &descriptor,
                      std::span<u32> dynamicOffsets) override;

  void bindVertexBuffers(const std::span<const BufferHandle> buffers,
                         const std::span<const u64> offsets) override;

  void bindIndexBuffer(BufferHandle buffer, IndexType indexType) override;

  void pushConstants(PipelineHandle pipeline, ShaderStage shaderStage,
                     u32 offset, u32 size, void *data) override;

  void draw(u32 vertexCount, u32 firstVertex, u32 instanceCount,
            u32 firstInstance) override;

  void drawIndexed(u32 indexCount, u32 firstIndex, i32 vertexOffset,
                   u32 instanceCount, u32 firstInstance) override;

  void dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) override;

  void setViewport(const glm::vec2 &offset, const glm::vec2 &size,
                   const glm::vec2 &depthRange) override;

  void setScissor(const glm::ivec2 &offset, const glm::uvec2 &size) override;

  void pipelineBarrier(std::span<MemoryBarrier> memoryBarriers,
                       std::span<ImageBarrier> imageBarriers,
                       std::span<BufferBarrier> bufferBarriers) override;

  void copyTextureToBuffer(TextureHandle srcTexture, BufferHandle dstBuffer,
                           std::span<CopyRegion> copyRegions) override;

  void copyBufferToTexture(BufferHandle srcBuffer, TextureHandle dstTexture,
                           std::span<CopyRegion> copyRegions) override;

  void blitTexture(TextureHandle source, TextureHandle destination,
                   std::span<BlitRegion> regions, Filter filter) override;

private:
  VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;

  const VulkanResourceRegistry &mRegistry;
  const VulkanDescriptorPool &mDescriptorPool;
  DeviceStats &mStats;
};

} // namespace quoll::rhi
