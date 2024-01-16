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

enum class BufferAllocationUsage : u8 {
  None = 0,
  HostWrite = 1 << 0,
  HostRead = 1 << 1
};

EnableBitwiseEnum(BufferAllocationUsage);

struct BufferDescription {
  BufferUsage usage = rhi::BufferUsage::Vertex;

  usize size = 0;

  const void *data = nullptr;

  BufferAllocationUsage allocationUsage = rhi::BufferAllocationUsage::HostWrite;

  bool mapped = false;

  String debugName;
};

} // namespace quoll::rhi
