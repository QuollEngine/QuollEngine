#pragma once

namespace quoll::rhi {

enum class Access : u64 {
  None = 0,
  IndirectCommandRead = 0x00000001ULL,
  IndexRead = 0x00000002ULL,
  VertexAttributeRead = 0x00000004ULL,
  UniformRead = 0x00000008ULL,
  InputAttachmentRead = 0x00000010ULL,
  ShaderRead = 0x00000020ULL,
  ShaderWrite = 0x00000040ULL,
  ColorAttachmentRead = 0x00000080ULL,
  ColorAttachmentWrite = 0x00000100ULL,
  DepthStencilAttachmentRead = 0x00000200ULL,
  DepthStencilAttachmentWrite = 0x00000400ULL,
  TransferRead = 0x00000800ULL,
  TransferWrite = 0x00001000ULL,
  HostRead = 0x00002000ULL,
  HostWrite = 0x00004000ULL,
  MemoryRead = 0x00008000ULL,
  MemoryWrite = 0x00010000ULL,
  ShaderSampledRead = 0x100000000ULL,
  ShaderStorageRead = 0x200000000ULL,
  ShaderStorageWrite = 0x400000000ULL
};

EnableBitwiseEnum(Access)

} // namespace quoll::rhi
