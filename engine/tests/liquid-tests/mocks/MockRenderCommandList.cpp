#include "liquid/core/Base.h"
#include "MockRenderCommandList.h"

void MockRenderCommandList::beginRenderPass(
    liquid::rhi::RenderPassHandle renderPass,
    liquid::rhi::FramebufferHandle framebuffer,
    const glm::ivec2 &renderAreaOffset, const glm::uvec2 &renderAreaSize) {}

void MockRenderCommandList::endRenderPass() {}

void MockRenderCommandList::bindPipeline(liquid::rhi::PipelineHandle pipeline) {
}

void MockRenderCommandList::bindDescriptor(
    liquid::rhi::PipelineHandle pipeline, uint32_t firstSet,
    const liquid::rhi::Descriptor &descriptor) {}

void MockRenderCommandList::bindVertexBuffer(liquid::rhi::BufferHandle buffer) {
}

void MockRenderCommandList::bindIndexBuffer(liquid::rhi::BufferHandle buffer,
                                            liquid::rhi::IndexType indexType) {}

void MockRenderCommandList::pushConstants(liquid::rhi::PipelineHandle pipeline,
                                          liquid::rhi::ShaderStage shaderStage,
                                          uint32_t offset, uint32_t size,
                                          void *data) {}

void MockRenderCommandList::draw(uint32_t vertexCount, uint32_t firstVertex,
                                 uint32_t instanceCount,
                                 uint32_t firstInstance) {}

void MockRenderCommandList::drawIndexed(uint32_t indexCount,
                                        uint32_t firstIndex,
                                        int32_t vertexOffset,
                                        uint32_t instanceCount,
                                        uint32_t firstInstance) {}

void MockRenderCommandList::dispatch(uint32_t groupCountX, uint32_t groupCountY,
                                     uint32_t groupCountZ) {}

void MockRenderCommandList::setViewport(const glm::vec2 &offset,
                                        const glm::vec2 &size,
                                        const glm::vec2 &depthRange) {}

void MockRenderCommandList::setScissor(const glm::ivec2 &offset,
                                       const glm::uvec2 &size) {}

void MockRenderCommandList::pipelineBarrier(
    liquid::rhi::PipelineStage srcStage, liquid::rhi::PipelineStage dstStage,
    const std::vector<liquid::rhi::MemoryBarrier> &memoryBarriers,
    const std::vector<liquid::rhi::ImageBarrier> &imageBarriers) {}

void MockRenderCommandList::copyTextureToBuffer(
    liquid::rhi::TextureHandle srcTexture, liquid::rhi::BufferHandle dstBuffer,
    const std::vector<liquid::rhi::CopyRegion> &copyRegions) {}

void MockRenderCommandList::copyBufferToTexture(
    liquid::rhi::BufferHandle srcBuffer, liquid::rhi::TextureHandle dstTexture,
    const std::vector<liquid::rhi::CopyRegion> &copyRegions) {}

void MockRenderCommandList::blitTexture(
    liquid::rhi::TextureHandle source, liquid::rhi::TextureHandle destination,
    const std::vector<liquid::rhi::BlitRegion> &regions,
    liquid::rhi::Filter filter) {}
