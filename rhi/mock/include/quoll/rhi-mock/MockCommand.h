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
  BlitTexture,
  Timestamp
};

struct MockCommand {};

template <MockCommandType Type> struct MockCommandTyped : public MockCommand {
  const MockCommandType type = Type;
};

struct MockCommandBeginRenderPass
    : public MockCommandTyped<MockCommandType::BeginRenderPass> {
  rhi::RenderPassHandle renderPass;

  rhi::FramebufferHandle framebuffer;

  glm::ivec2 renderAreaOffset;

  glm::uvec2 renderAreaSize;
};

struct MockCommandEndRenderPass
    : public MockCommandTyped<MockCommandType::EndRenderPass> {};

struct MockCommandBindPipeline
    : public MockCommandTyped<MockCommandType::BindPipeline> {
  PipelineHandle pipeline;
};

struct MockCommandBindDescriptor
    : public MockCommandTyped<MockCommandType::BindDescriptor> {
  PipelineHandle pipeline;

  u32 firstSet;

  Descriptor descriptor;

  std::vector<u32> dynamicOffsets;
};

struct MockCommandBindVertexBuffer
    : public MockCommandTyped<MockCommandType::BindVertexBuffer> {
  std::vector<BufferHandle> buffers;

  std::vector<u64> offsets;
};

struct MockCommandBindIndexBuffer
    : public MockCommandTyped<MockCommandType::BindIndexBuffer> {
  BufferHandle buffer;

  IndexType indexType;
};

struct MockCommandPushConstants
    : public MockCommandTyped<MockCommandType::PushConstants> {
  PipelineHandle pipeline;

  ShaderStage shaderStage;

  u32 offset;

  u32 size;

  void *data;
};

struct MockCommandDraw : public MockCommandTyped<MockCommandType::Draw> {
  u32 vertexCount;

  u32 firstVertex;

  u32 instanceCount;

  u32 firstInstance;
};

struct MockCommandDrawIndexed
    : public MockCommandTyped<MockCommandType::DrawIndexed> {
  u32 indexCount;

  u32 firstIndex;

  i32 vertexOffset;

  u32 instanceCount;

  u32 firstInstance;
};

struct MockCommandDispatch
    : public MockCommandTyped<MockCommandType::Dispatch> {
  u32 groupCountX;

  u32 groupCountY;

  u32 groupCountZ;
};

struct MockCommandSetViewport
    : public MockCommandTyped<MockCommandType::SetViewport> {
  glm::vec2 offset;

  glm::vec2 size;

  glm::vec2 depthRange;
};

struct MockCommandSetScissor
    : public MockCommandTyped<MockCommandType::SetScissor> {
  glm::ivec2 offset;

  glm::uvec2 size;
};

struct MockCommandPipelineBarrier
    : public MockCommandTyped<MockCommandType::PipelineBarrier> {
  PipelineStage srcStage;

  PipelineStage dstStage;

  std::vector<MemoryBarrier> memoryBarriers;

  std::vector<ImageBarrier> imageBarriers;

  std::vector<BufferBarrier> bufferBarriers;
};

struct MockCommandCopyTextureToBuffer
    : public MockCommandTyped<MockCommandType::CopyTextureToBuffer> {
  TextureHandle srcTexture;

  BufferHandle dstBuffer;

  std::vector<CopyRegion> copyRegions;
};

struct MockCommandCopyBufferToTexture
    : public MockCommandTyped<MockCommandType::CopyBufferToTexture> {
  BufferHandle srcBuffer;

  TextureHandle dstTexture;

  std::vector<CopyRegion> copyRegions;
};

struct MockCommandBlitTexture
    : public MockCommandTyped<MockCommandType::BlitTexture> {
  TextureHandle source;

  TextureHandle destination;

  std::vector<BlitRegion> regions;

  Filter filter;
};

struct MockCommandTimestamp
    : public MockCommandTyped<MockCommandType::Timestamp> {
  u32 queryIndex = std::numeric_limits<u32>::max();
  PipelineStage stage;
};

} // namespace quoll::rhi
