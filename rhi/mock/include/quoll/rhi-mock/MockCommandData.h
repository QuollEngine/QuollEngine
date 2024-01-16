#pragma once

namespace quoll::rhi {

struct MockBindings {
  PipelineHandle pipeline = PipelineHandle::Null;

  std::unordered_map<u32, Descriptor> descriptors{};

  RenderPassHandle renderPass = RenderPassHandle::Null;

  std::vector<BufferHandle> vertexBuffers;

  BufferHandle indexBuffer = BufferHandle::Null;

  IndexType indexType = IndexType::Uint16;
};

enum class DrawCallType { Draw, DrawIndexed };

struct MockDrawCall {
  MockBindings bindings;

  DrawCallType type = DrawCallType::Draw;

  MockCommand *command = nullptr;
};

struct MockDispatchCall {
  MockBindings bindings;

  u32 groupCountX = 0;

  u32 groupCountY = 0;

  u32 groupCountZ = 0;
};

} // namespace quoll::rhi
