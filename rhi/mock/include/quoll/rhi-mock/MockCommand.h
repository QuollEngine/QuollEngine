#pragma once

namespace quoll::rhi {

enum class MockCommandType {
  BeginRenderPass,
  EndRenderPass,
  BindPipeline,
  BindDescriptor,
  BindVertexBuffer,
  BindIndexBuffer,
  PushConstants,
  Draw,
  DrawIndexed,
  Dispatch,
  SetViewport,
  SetScissor,
  PipelineBarrier,
  CopyTextureToBuffer,
  CopyBufferToTexture,
  BlitTexture
};

/**
 * @brief Mock command
 *
 * Base command
 */
struct MockCommand {};

/**
 * @brief Typed mock command
 *
 * @tparam Type Command type
 */
template <MockCommandType Type> struct MockCommandTyped : public MockCommand {
  /**
   * Command type
   */
  const MockCommandType type = Type;
};

/**
 * @brief Begin render pass command
 */
struct MockCommandBeginRenderPass
    : public MockCommandTyped<MockCommandType::BeginRenderPass> {
  /**
   * Render pass
   */
  rhi::RenderPassHandle renderPass;

  /**
   * Framebuffer
   */
  rhi::FramebufferHandle framebuffer;

  /**
   * Render area offset
   */
  glm::ivec2 renderAreaOffset;

  /**
   * Render area size
   */
  glm::uvec2 renderAreaSize;
};

/**
 * @brief End render pass command
 */
struct MockCommandEndRenderPass
    : public MockCommandTyped<MockCommandType::EndRenderPass> {};

/**
 * @brief Bind pipeline command
 */
struct MockCommandBindPipeline
    : public MockCommandTyped<MockCommandType::BindPipeline> {
  /**
   * Pipeline
   */
  PipelineHandle pipeline;
};

/**
 * @brief Bind descriptor command
 */
struct MockCommandBindDescriptor
    : public MockCommandTyped<MockCommandType::BindDescriptor> {
  /**
   * Pipeline
   */
  PipelineHandle pipeline;

  /**
   * First descriptor set
   */
  u32 firstSet;

  /**
   * Descriptor
   */
  Descriptor descriptor;

  /**
   * Dynamic offsets
   */
  std::vector<u32> dynamicOffsets;
};

/**
 * @brief Bind vertex buffer command
 */
struct MockCommandBindVertexBuffer
    : public MockCommandTyped<MockCommandType::BindVertexBuffer> {
  /**
   * Vertex buffer
   */
  std::vector<BufferHandle> buffers;

  /**
   * Vertex buffer binding offsets
   */
  std::vector<u64> offsets;
};

/**
 * @brief Bind index buffer command
 */
struct MockCommandBindIndexBuffer
    : public MockCommandTyped<MockCommandType::BindIndexBuffer> {
  /**
   * Index buffer
   */
  BufferHandle buffer;

  /**
   * Index buffer type
   */
  IndexType indexType;
};

/**
 * @brief Push constants command
 */
struct MockCommandPushConstants
    : public MockCommandTyped<MockCommandType::PushConstants> {
  /**
   * Pipeline
   */
  PipelineHandle pipeline;

  /**
   * Shader stage
   */
  ShaderStage shaderStage;

  /**
   * Offset
   */
  u32 offset;

  /**
   * Size
   */
  u32 size;

  /**
   * Data
   */
  void *data;
};

/**
 * @brief Draw command
 */
struct MockCommandDraw : public MockCommandTyped<MockCommandType::Draw> {
  /**
   * Vertex count
   */
  u32 vertexCount;

  /**
   * First vertex
   */
  u32 firstVertex;

  /**
   * Instance count
   */
  u32 instanceCount;

  /**
   * First instance
   */
  u32 firstInstance;
};

/**
 * @brief Draw indexed command
 */
struct MockCommandDrawIndexed
    : public MockCommandTyped<MockCommandType::DrawIndexed> {
  /**
   * Index count
   */
  u32 indexCount;

  /**
   * First index
   */
  u32 firstIndex;

  /**
   * Vertex offset
   */
  i32 vertexOffset;

  /**
   * Instance count
   */
  u32 instanceCount;

  /**
   * First instance
   */
  u32 firstInstance;
};

/**
 * @brief Dispatch command
 */
struct MockCommandDispatch
    : public MockCommandTyped<MockCommandType::Dispatch> {
  /**
   * Group count X
   */
  u32 groupCountX;

  /**
   * Group count Y
   */
  u32 groupCountY;

  /**
   * Group count Z
   */
  u32 groupCountZ;
};

/**
 * @brief Set viewport command
 */
struct MockCommandSetViewport
    : public MockCommandTyped<MockCommandType::SetViewport> {
  /**
   * Viewport offset
   */
  glm::vec2 offset;

  /**
   * Viewport size
   */
  glm::vec2 size;

  /**
   * Depth range
   */
  glm::vec2 depthRange;
};

/**
 * @brief Set scissor command
 */
struct MockCommandSetScissor
    : public MockCommandTyped<MockCommandType::SetScissor> {
  /**
   * Scissor offset
   */
  glm::ivec2 offset;

  /**
   * Scissor size
   */
  glm::uvec2 size;
};

/**
 * @brief Pipeline barrier command
 */
struct MockCommandPipelineBarrier
    : public MockCommandTyped<MockCommandType::PipelineBarrier> {
  /**
   * Source stage
   */
  PipelineStage srcStage;

  /**
   * Destination stage
   */
  PipelineStage dstStage;

  /**
   * Memory barriers
   */
  std::vector<MemoryBarrier> memoryBarriers;

  /**
   * Image barriers
   */
  std::vector<ImageBarrier> imageBarriers;

  /**
   * Buffer barriers
   */
  std::vector<BufferBarrier> bufferBarriers;
};

/**
 * @brief Copy texture to buffer command
 */
struct MockCommandCopyTextureToBuffer
    : public MockCommandTyped<MockCommandType::CopyTextureToBuffer> {
  /**
   * Source texture
   */
  TextureHandle srcTexture;

  /**
   * Destination buffer
   */
  BufferHandle dstBuffer;

  /**
   * Copy regions
   */
  std::vector<CopyRegion> copyRegions;
};

/**
 * @brief Copy bufer to texture command
 */
struct MockCommandCopyBufferToTexture
    : public MockCommandTyped<MockCommandType::CopyBufferToTexture> {
  /**
   * Source buffer
   */
  BufferHandle srcBuffer;

  /**
   * Destination texture
   */
  TextureHandle dstTexture;

  /**
   * Copy regions
   */
  std::vector<CopyRegion> copyRegions;
};

/**
 * Blit command
 */
struct MockCommandBlitTexture
    : public MockCommandTyped<MockCommandType::BlitTexture> {
  /**
   * Source texture
   */
  TextureHandle source;

  /**
   * Destination texture
   */
  TextureHandle destination;

  /**
   * Blit regions
   */
  std::vector<BlitRegion> regions;

  /**
   * Filter
   */
  Filter filter;
};

} // namespace quoll::rhi
