#pragma once

namespace liquid::rhi {

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
  uint32_t firstSet;

  /**
   * Descriptor
   */
  Descriptor descriptor;

  /**
   * Dynamic offsets
   */
  std::vector<uint32_t> dynamicOffsets;
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
  std::vector<uint64_t> offsets;
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
  uint32_t offset;

  /**
   * Size
   */
  uint32_t size;

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
  uint32_t vertexCount;

  /**
   * First vertex
   */
  uint32_t firstVertex;

  /**
   * Instance count
   */
  uint32_t instanceCount;

  /**
   * First instance
   */
  uint32_t firstInstance;
};

/**
 * @brief Draw indexed command
 */
struct MockCommandDrawIndexed
    : public MockCommandTyped<MockCommandType::DrawIndexed> {
  /**
   * Index count
   */
  uint32_t indexCount;

  /**
   * First index
   */
  uint32_t firstIndex;

  /**
   * Vertex offset
   */
  int32_t vertexOffset;

  /**
   * Instance count
   */
  uint32_t instanceCount;

  /**
   * First instance
   */
  uint32_t firstInstance;
};

/**
 * @brief Dispatch command
 */
struct MockCommandDispatch
    : public MockCommandTyped<MockCommandType::Dispatch> {
  /**
   * Group count X
   */
  uint32_t groupCountX;

  /**
   * Group count Y
   */
  uint32_t groupCountY;

  /**
   * Group count Z
   */
  uint32_t groupCountZ;
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

} // namespace liquid::rhi
