#pragma once

namespace quoll::rhi {

enum class ShaderStage {
  Vertex = 0x00000001,
  Fragment = 0x00000010,
  Compute = 0x00000020,
  AllGraphics = 0x0000001F,
  All = 0x7FFFFFFF
};

EnableBitwiseEnum(ShaderStage);

enum class PipelineStage : u64 {
  None = 0ULL,
  PipeTop = 0x00000001ULL,
  PipeBottom = 0x00002000ULL,
  DrawIndirect = 0x00000002ULL,
  VertexShader = 0x00000008ULL,
  FragmentShader = 0x00000080ULL,
  EarlyFragmentTests = 0x00000100ULL,
  LateFragmentTests = 0x00000200ULL,
  ColorAttachmentOutput = 0x00000400ULL,
  ComputeShader = 0x00000800ULL,
  Transfer = 0x00001000ULL,
  Host = 0x00004000ULL,
  AllGraphics = 0x00008000ULL,
  AllCommands = 0x00010000ULL,
  Copy = 0x100000000ULL,
  Resolve = 0x200000000ULL,
  Blit = 0x400000000ULL,
  Clear = 0x800000000ULL,
  IndexInput = 0x1000000000ULL,
  VertexAttributeInput = 0x2000000000ULL
};

EnableBitwiseEnum(PipelineStage);

} // namespace quoll::rhi
