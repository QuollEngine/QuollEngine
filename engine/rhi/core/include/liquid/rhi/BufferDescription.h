#pragma once

namespace quoll::rhi {

enum class BufferUsage {
  None = 0,
  Vertex = 1 << 0,
  Index = 1 << 1,
  Uniform = 1 << 2,
  Storage = 1 << 3,
  Indirect = 1 << 4,
  TransferSource = 1 << 5,
  TransferDestination = 1 << 6
};

EnableBitwiseEnum(BufferUsage);

enum class BufferAllocationUsage : uint8_t {
  None = 0,
  HostWrite = 1 << 0,
  HostRead = 1 << 1
};

EnableBitwiseEnum(BufferAllocationUsage);

/**
 * @brief Buffer description
 */
struct BufferDescription {
  /**
   * Buffer usage
   */
  BufferUsage usage = rhi::BufferUsage::Vertex;

  /**
   * Buffer size
   */
  size_t size = 0;

  /**
   * Buffer data
   */
  const void *data = nullptr;

  /**
   * Buffer allocation usage
   */
  BufferAllocationUsage allocationUsage = rhi::BufferAllocationUsage::HostWrite;

  /**
   * @brief Keep buffer always mapped
   */
  bool mapped = false;

  /**
   * Debug name
   */
  String debugName;
};

} // namespace quoll::rhi
