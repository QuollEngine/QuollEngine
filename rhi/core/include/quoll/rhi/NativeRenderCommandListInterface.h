#pragma once

#include "quoll/rhi/BlitRegion.h"
#include "quoll/rhi/CopyRegion.h"
#include "quoll/rhi/Descriptor.h"
#include "quoll/rhi/Filter.h"
#include "quoll/rhi/IndexType.h"
#include "quoll/rhi/PipelineBarrier.h"
#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/StageFlags.h"

namespace quoll::rhi {

class NativeRenderCommandListInterface {
public:
  NativeRenderCommandListInterface() = default;

  virtual ~NativeRenderCommandListInterface() = default;

  NativeRenderCommandListInterface(const NativeRenderCommandListInterface &) =
      delete;
  NativeRenderCommandListInterface &
  operator=(const NativeRenderCommandListInterface &) = delete;
  NativeRenderCommandListInterface(NativeRenderCommandListInterface &&) =
      delete;
  NativeRenderCommandListInterface &
  operator=(NativeRenderCommandListInterface &&) = delete;

  virtual void beginRenderPass(rhi::RenderPassHandle renderPass,
                               FramebufferHandle framebuffer,
                               const glm::ivec2 &renderAreaOffset,
                               const glm::uvec2 &renderAreaSize) = 0;

  virtual void endRenderPass() = 0;

  virtual void bindPipeline(PipelineHandle pipeline) = 0;

  virtual void bindDescriptor(PipelineHandle pipeline, u32 firstSet,
                              const Descriptor &descriptor,
                              std::span<u32> dynamicOffsets) = 0;

  virtual void bindVertexBuffers(const std::span<const BufferHandle> buffers,
                                 const std::span<const u64> offsets) = 0;

  virtual void bindIndexBuffer(BufferHandle buffer, IndexType indexType) = 0;

  virtual void pushConstants(PipelineHandle pipeline, ShaderStage shaderStage,
                             u32 offset, u32 size, void *data) = 0;

  virtual void draw(u32 vertexCount, u32 firstVertex, u32 instanceCount,
                    u32 firstInstance) = 0;

  virtual void drawIndexed(u32 indexCount, u32 firstIndex, i32 vertexOffset,
                           u32 instanceCount, u32 firstInstance) = 0;

  virtual void dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) = 0;

  virtual void setViewport(const glm::vec2 &offset, const glm::vec2 &size,
                           const glm::vec2 &depthRange) = 0;

  virtual void setScissor(const glm::ivec2 &offset, const glm::uvec2 &size) = 0;

  virtual void pipelineBarrier(std::span<MemoryBarrier> memoryBarriers,
                               std::span<ImageBarrier> imageBarriers,
                               std::span<BufferBarrier> bufferBarriers) = 0;

  virtual void copyTextureToBuffer(TextureHandle srcTexture,
                                   BufferHandle dstBuffer,
                                   std::span<CopyRegion> copyRegions) = 0;

  virtual void copyBufferToTexture(BufferHandle srcBuffer,
                                   TextureHandle dstTexture,
                                   std::span<CopyRegion> copyRegions) = 0;

  virtual void blitTexture(TextureHandle source, TextureHandle destination,
                           std::span<BlitRegion> regions, Filter filter) = 0;

  virtual void writeTimestamp(u32 timestamp, PipelineStage stage) = 0;
};

} // namespace quoll::rhi
