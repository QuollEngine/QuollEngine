#pragma once

namespace liquid::rhi {

enum class ShaderStage {
  Vertex = 0x00000001,
  Fragment = 0x00000010,
  Compute = 0x00000020,
  AllGraphics = 0x0000001F,
  All = 0x7FFFFFFF
};

EnableBitwiseEnum(ShaderStage);

enum class PipelineStage {
  PipeTop = 0x00000001,
  DrawIndirect = 0x00000002,
  VertexInput = 0x00000004,
  VertexShader = 0x00000008,
  FragmentShader = 0x00000080,
  EarlyFragmentTests = 0x00000100,
  LateFragmentTests = 0x00000200,
  ColorAttachmentOutput = 0x00000400,
  ComputeShader = 0x00000800,
  Transfer = 0x00001000,
  PipeBottom = 0x00002000,
  Host = 0x00004000,
  AllGraphics = 0x00008000,
  AllCommands = 0x00010000,
  None = 0
};

EnableBitwiseEnum(PipelineStage);

} // namespace liquid::rhi
