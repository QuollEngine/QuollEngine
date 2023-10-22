#pragma once

namespace quoll::rhi {

/**
 * @brief Mock command bindings
 */
struct MockBindings {
  /**
   * Pipeline
   */
  PipelineHandle pipeline = PipelineHandle::Null;

  /**
   * Descriptors
   */
  std::unordered_map<u32, Descriptor> descriptors{};

  /**
   * Render pass
   *
   * Graphics specific
   */
  RenderPassHandle renderPass = RenderPassHandle::Null;

  /**
   * Vertex buffers
   *
   * Graphics specific
   */
  std::vector<BufferHandle> vertexBuffers;

  /**
   * Index buffer
   *
   * Graphics specific
   */
  BufferHandle indexBuffer = BufferHandle::Null;

  /**
   * Index buffer type
   *
   * Graphics specific
   */
  IndexType indexType = IndexType::Uint16;
};

enum class DrawCallType { Draw, DrawIndexed };

/**
 * @brief Mock draw call
 */
struct MockDrawCall {
  /**
   * Bindings
   */
  MockBindings bindings;

  /**
   * Draw call type
   */
  DrawCallType type = DrawCallType::Draw;

  /**
   * Mock command
   */
  MockCommand *command = nullptr;
};

/**
 * @brief Mock dispatch call
 */
struct MockDispatchCall {
  /**
   * Bindings
   */
  MockBindings bindings;

  /**
   * Group count x
   */
  u32 groupCountX = 0;

  /**
   * Group count y
   */
  u32 groupCountY = 0;

  /**
   * Group count z
   */
  u32 groupCountZ = 0;
};

} // namespace quoll::rhi
