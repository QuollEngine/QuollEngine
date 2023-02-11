#pragma once

#include "liquid/rhi/NativeRenderCommandListInterface.h"

class MockRenderCommandList
    : public liquid::rhi::NativeRenderCommandListInterface {

public:
  void beginRenderPass(liquid::rhi::RenderPassHandle renderPass,
                       liquid::rhi::FramebufferHandle framebuffer,
                       const glm::ivec2 &renderAreaOffset,
                       const glm::uvec2 &renderAreaSize);

  void endRenderPass();

  void bindPipeline(liquid::rhi::PipelineHandle pipeline);

  void bindDescriptor(liquid::rhi::PipelineHandle pipeline, uint32_t firstSet,
                      const liquid::rhi::Descriptor &descriptor);

  void bindVertexBuffer(liquid::rhi::BufferHandle buffer);

  void bindIndexBuffer(liquid::rhi::BufferHandle buffer,
                       liquid::rhi::IndexType indexType);

  void pushConstants(liquid::rhi::PipelineHandle pipeline,
                     liquid::rhi::ShaderStage shaderStage, uint32_t offset,
                     uint32_t size, void *data);

  void draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount,
            uint32_t firstInstance);

  void drawIndexed(uint32_t indexCount, uint32_t firstIndex,
                   int32_t vertexOffset, uint32_t instanceCount,
                   uint32_t firstInstance);

  void dispatch(uint32_t groupCountX, uint32_t groupCountY,
                uint32_t groupCountZ);

  void setViewport(const glm::vec2 &offset, const glm::vec2 &size,
                   const glm::vec2 &depthRange);

  void setScissor(const glm::ivec2 &offset, const glm::uvec2 &size);

  void
  pipelineBarrier(liquid::rhi::PipelineStage srcStage,
                  liquid::rhi::PipelineStage dstStage,
                  const std::vector<liquid::rhi::MemoryBarrier> &memoryBarriers,
                  const std::vector<liquid::rhi::ImageBarrier> &imageBarriers);

  void
  copyTextureToBuffer(liquid::rhi::TextureHandle srcTexture,
                      liquid::rhi::BufferHandle dstBuffer,
                      const std::vector<liquid::rhi::CopyRegion> &copyRegions);

  void
  copyBufferToTexture(liquid::rhi::BufferHandle srcBuffer,
                      liquid::rhi::TextureHandle dstTexture,
                      const std::vector<liquid::rhi::CopyRegion> &copyRegions);

  void blitTexture(liquid::rhi::TextureHandle source,
                   liquid::rhi::TextureHandle destination,
                   const std::vector<liquid::rhi::BlitRegion> &regions,
                   liquid::rhi::Filter filter);
};