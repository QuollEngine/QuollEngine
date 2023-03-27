#pragma once

namespace liquid::rhi {

/**
 * @brief Mock command bindings
 */
struct MockBindings {
  /**
   * Pipeline
   */
  PipelineHandle pipeline = PipelineHandle::Invalid;

  /**
   * Descriptors
   */
  std::unordered_map<uint32_t, Descriptor> descriptors{};

  /**
   * Render pass
   *
   * Graphics specific
   */
  RenderPassHandle renderPass = RenderPassHandle::Invalid;

  /**
   * Vertex buffer
   *
   * Graphics specific
   */
  BufferHandle vertexBuffer = BufferHandle::Invalid;

  /**
   * Index buffer
   *
   * Graphics specific
   */
  BufferHandle indexBuffer = BufferHandle::Invalid;

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
  uint32_t groupCountX = 0;

  /**
   * Group count y
   */
  uint32_t groupCountY = 0;

  /**
   * Group count z
   */
  uint32_t groupCountZ = 0;
};

} // namespace liquid::rhi
