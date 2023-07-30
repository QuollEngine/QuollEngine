#pragma once

#include "liquid/rhi/NativeRenderCommandListInterface.h"
#include "MockCommand.h"
#include "MockCommandData.h"

namespace liquid::rhi {

/**
 * @brief Mock command list
 *
 * Provides statistical data for recorded commands
 */
class MockCommandList : public NativeRenderCommandListInterface {
public:
  /**
   * @brief Create mock command list
   */
  MockCommandList() = default;

  /**
   * @brief Move constructor
   *
   * @param rhs Other mock command list
   */
  MockCommandList(MockCommandList &&rhs);

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
  void bindDescriptor(PipelineHandle pipeline, uint32_t firstSet,
                      const Descriptor &descriptor,
                      std::span<uint32_t> dynamicOffsets) override;

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
                     uint32_t offset, uint32_t size, void *data) override;

  /**
   * @brief Draw
   *
   * @param vertexCount Vertex count
   * @param firstVertex First vertex
   * @param instanceCount Instance count
   * @param firstInstance First instance
   */
  void draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount,
            uint32_t firstInstance) override;

  /**
   * @brief Draw indexed
   *
   * @param indexCount Index count
   * @param firstIndex Offset of first index
   * @param vertexOffset Vertex offset
   * @param instanceCount Instance count
   * @param firstInstance First instance
   */
  void drawIndexed(uint32_t indexCount, uint32_t firstIndex,
                   int32_t vertexOffset, uint32_t instanceCount,
                   uint32_t firstInstance) override;

  /**
   * @brief Dispatch compute work
   *
   * @param groupCountX Number of groups to dispatch in X direction
   * @param groupCountY Number of groups to dispatch in Y direction
   * @param groupCountZ Number of groups to dispatch in Z direction
   */
  void dispatch(uint32_t groupCountX, uint32_t groupCountY,
                uint32_t groupCountZ) override;

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

  /**
   * @brief Get recorded commands
   *
   * @return Recorded commands
   */
  inline const std::vector<std::unique_ptr<MockCommand>> &getCommands() const {
    return mCommands;
  }

  /**
   * @brief Get recorded draw calls
   *
   * @return Recorded draw calls
   */
  inline const std::vector<MockDrawCall> &getDrawCalls() const {
    return mDrawCalls;
  }

  /**
   * @brief Get recorded dispatch calls
   *
   * @return Recorded dispatch calls
   */
  inline const std::vector<MockDispatchCall> &getDispatchCalls() const {
    return mDispatchCalls;
  }

  /**
   * @brief Clear command list
   */
  void clear();

private:
  MockBindings mBindings;

  std::vector<std::unique_ptr<MockCommand>> mCommands;
  std::vector<MockDrawCall> mDrawCalls;
  std::vector<MockDispatchCall> mDispatchCalls;
};

} // namespace liquid::rhi
