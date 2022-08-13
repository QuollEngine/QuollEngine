#pragma once

namespace liquid::rhi {

enum class BufferType {
  Vertex,
  Index,
  Uniform,
  Storage,
  TransferSource,
  TransferDestination
};

enum class BufferUsage : uint8_t {
  None = 0,
  HostWrite = 1 << 0,
  HostRead = 1 << 1
};

constexpr inline BufferUsage operator|(BufferUsage a, BufferUsage b) {
  return BufferUsage(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr inline BufferUsage operator&(BufferUsage a, BufferUsage b) {
  return BufferUsage(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

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

  /**
   * @brief Buffer usage
   */
  BufferUsage usage = rhi::BufferUsage::HostWrite;
};

} // namespace liquid::rhi
