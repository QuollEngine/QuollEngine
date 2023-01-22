#pragma once

namespace liquid::rhi {

enum class BufferType {
  None = 0,
  Vertex = 1 << 0,
  Index = 1 << 1,
  Uniform = 1 << 2,
  Storage = 1 << 3,
  Indirect = 1 << 4,
  TransferSource = 1 << 5,
  TransferDestination = 1 << 6
};

EnableBitwiseEnum(BufferType);

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
