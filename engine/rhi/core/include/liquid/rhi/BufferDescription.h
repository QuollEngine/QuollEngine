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

EnableBitwiseEnum(BufferUsage);

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
  const void *data = nullptr;

  /**
   * @brief Buffer usage
   */
  BufferUsage usage = rhi::BufferUsage::HostWrite;

  /**
   * @brief Keep buffer always mapped
   */
  bool mapped = false;
};

} // namespace liquid::rhi
