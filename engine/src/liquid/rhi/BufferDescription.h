#pragma once

namespace liquid {

enum class BufferType { Vertex, Index, Uniform, Transfer };

struct BufferDescription {
  BufferType type = BufferType::Vertex;
  size_t size = 0;
  void *data = nullptr;
};

} // namespace liquid
