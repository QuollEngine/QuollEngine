#pragma once

namespace liquid::rhi {

enum class BufferType { Vertex, Index, Uniform, Transfer };

struct BufferDescription {
  BufferType type = rhi::BufferType::Vertex;
  size_t size = 0;
  void *data = nullptr;
};

} // namespace liquid::rhi
