#pragma once

namespace liquid::rhi {

enum class BufferType { Vertex, Index, Uniform, Storage, Transfer };

/**
 * @brief Buffer description
 */
struct BufferDescription {
  /**
   * Buffer type
   */
  BufferType type = rhi::BufferType::Vertex;

  /**
   * Buffer size
   */
  size_t size = 0;

  /**
   * Buffer data
   */
  void *data = nullptr;
};

} // namespace liquid::rhi
