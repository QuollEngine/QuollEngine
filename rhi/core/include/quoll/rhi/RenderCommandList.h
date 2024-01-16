#pragma once

#include "NativeRenderCommandListInterface.h"

namespace quoll::rhi {

class RenderCommandList {
public:
  RenderCommandList() = default;

  ~RenderCommandList() = default;

  RenderCommandList(NativeRenderCommandListInterface *nativeRenderCommandList)
      : mNativeRenderCommandList(nativeRenderCommandList) {}

  RenderCommandList(RenderCommandList &&rhs) noexcept {
    mNativeRenderCommandList = std::move(rhs.mNativeRenderCommandList);
  }

  RenderCommandList &operator=(RenderCommandList &&rhs) noexcept {
    mNativeRenderCommandList = std::move(rhs.mNativeRenderCommandList);
    return *this;
  }

  RenderCommandList(const RenderCommandList &) = delete;
  RenderCommandList &operator=(const RenderCommandList &) = delete;

  inline const std::unique_ptr<NativeRenderCommandListInterface> &
  getNativeRenderCommandList() const {
    return mNativeRenderCommandList;
  }

  inline void beginRenderPass(rhi::RenderPassHandle renderPass,
                              FramebufferHandle framebuffer,
                              const glm::ivec2 &renderAreaOffset,
                              const glm::uvec2 &renderAreaSize) {
    mNativeRenderCommandList->beginRenderPass(renderPass, framebuffer,
                                              renderAreaOffset, renderAreaSize);
  }

  inline void endRenderPass() { mNativeRenderCommandList->endRenderPass(); }

  void bindPipeline(PipelineHandle pipeline) {
    mNativeRenderCommandList->bindPipeline(pipeline);
  }

  inline void bindDescriptor(PipelineHandle pipeline, u32 firstSet,
                             const Descriptor &descriptor,
                             std::span<u32> dynamicOffsets = {}) {
    mNativeRenderCommandList->bindDescriptor(pipeline, firstSet, descriptor,
                                             dynamicOffsets);
  }

  inline void bindVertexBuffers(const std::span<const BufferHandle> buffers,
                                const std::span<const u64> offsets) {
    mNativeRenderCommandList->bindVertexBuffers(buffers, offsets);
  }

  inline void bindIndexBuffer(BufferHandle buffer, IndexType indexType) {
    mNativeRenderCommandList->bindIndexBuffer(buffer, indexType);
  }

  inline void pushConstants(PipelineHandle pipeline, ShaderStage shaderStage,
                            u32 offset, u32 size, void *data) {
    mNativeRenderCommandList->pushConstants(pipeline, shaderStage, offset, size,
                                            data);
  }

  inline void draw(u32 vertexCount, u32 firstVertex, u32 instanceCount = 1,
                   u32 firstInstance = 0) {
    mNativeRenderCommandList->draw(vertexCount, firstVertex, instanceCount,
                                   firstInstance);
  }

  inline void drawIndexed(u32 indexCount, u32 firstIndex, i32 vertexOffset,
                          u32 instanceCount = 1, u32 firstInstance = 0) {
    mNativeRenderCommandList->drawIndexed(indexCount, firstIndex, vertexOffset,
                                          instanceCount, firstInstance);
  }

  inline void dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) {
    mNativeRenderCommandList->dispatch(groupCountX, groupCountY, groupCountZ);
  }

  inline void setViewport(const glm::vec2 &offset, const glm::vec2 &size,
                          const glm::vec2 &depthRange) {
    mNativeRenderCommandList->setViewport(offset, size, depthRange);
  }

  inline void setScissor(const glm::ivec2 &offset, const glm::uvec2 &size) {
    mNativeRenderCommandList->setScissor(offset, size);
  }

  inline void pipelineBarrier(std::span<MemoryBarrier> memoryBarriers,
                              std::span<ImageBarrier> imageBarriers,
                              std::span<BufferBarrier> bufferBarriers) {
    mNativeRenderCommandList->pipelineBarrier(memoryBarriers, imageBarriers,
                                              bufferBarriers);
  }

  inline void copyTextureToBuffer(TextureHandle srcTexture,
                                  BufferHandle dstBuffer,
                                  std::span<CopyRegion> copyRegions) {
    mNativeRenderCommandList->copyTextureToBuffer(srcTexture, dstBuffer,
                                                  copyRegions);
  }

  inline void copyBufferToTexture(BufferHandle srcBuffer,
                                  TextureHandle dstTexture,
                                  std::span<CopyRegion> copyRegions) {
    mNativeRenderCommandList->copyBufferToTexture(srcBuffer, dstTexture,
                                                  copyRegions);
  }

  inline void blitTexture(TextureHandle source, TextureHandle destination,
                          std::span<BlitRegion> regions, Filter filter) {
    mNativeRenderCommandList->blitTexture(source, destination, regions, filter);
  }

private:
  std::unique_ptr<NativeRenderCommandListInterface> mNativeRenderCommandList;
};

} // namespace quoll::rhi
