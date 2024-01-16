#pragma once

#include "quoll/rhi/Format.h"
#include "RenderHandle.h"

namespace quoll::rhi {

enum class PrimitiveTopology {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,
  TriangleFan,
  ListListWithAdjacency,
  LineStripWithAdjacency,
  TriangleListWithAdjacency,
  TriangleStripWithAdjacency,
  PatchList
};

enum class PolygonMode { Fill, Line, Point };

enum class CullMode { None, Front, Back, FrontAndBack };

enum class FrontFace { Clockwise, CounterClockwise };

struct PipelineInputAssembly {
  PrimitiveTopology primitiveTopology = PrimitiveTopology::TriangleList;
};

struct PipelineRasterizer {
  PolygonMode polygonMode = PolygonMode::Fill;

  CullMode cullMode = CullMode::None;

  FrontFace frontFace = FrontFace::Clockwise;

  f32 lineWidth = 1.0f;
};

enum class BlendFactor {
  Zero,
  One,
  SrcColor,
  OneMinusSrcColor,
  DstColor,
  OneMinusDstColor,
  SrcAlpha,
  OneMinusSrcAlpha,
  DstAlpha,
  OneMinusDstAlpha,
  SrcAlphaSaturate,
  Src1Color,
  OneMinusSrc1Color,
  Src1Alpha,
  OneMinusSrc1Alpha,
};

enum class BlendOp { Add, Subtract, ReverseSubtract, Min, Max };

enum class VertexInputRate { Vertex, Instance };

enum class StencilOp {
  Keep,
  Zero,
  Replace,
  IncrementAndClamp,
  DecrementAndClamp,
  Invert,
  IncrementAndWrap,
  DecrementAndWrap
};

enum CompareOp {
  Never,
  Less,
  Equal,
  LessOrEqual,
  Greater,
  NotEqual,
  GreaterOrEqual,
  Always
};

struct PipelineVertexInputBinding {
  u32 binding = 0;

  u32 stride = 0;

  VertexInputRate inputRate = VertexInputRate::Vertex;
};

struct PipelineVertexInputAttribute {
  u32 slot = 0;

  u32 binding = 0;

  Format format = Format::Undefined;

  u32 offset = 0;
};

struct PipelineVertexInputLayout {
  std::vector<PipelineVertexInputBinding> bindings;

  std::vector<PipelineVertexInputAttribute> attributes;
};

struct PipelineColorBlendAttachment {
  bool enabled = false;

  BlendFactor srcColor = BlendFactor::Zero;

  BlendFactor dstColor = BlendFactor::Zero;

  BlendOp colorOp = BlendOp::Add;

  BlendFactor srcAlpha = BlendFactor::Zero;

  BlendFactor dstAlpha = BlendFactor::Zero;

  BlendOp alphaOp = BlendOp::Add;
};

struct PipelineColorBlend {
  std::vector<PipelineColorBlendAttachment> attachments;
};

struct PipelineStencil {
  StencilOp failOp = StencilOp::Keep;

  StencilOp passOp = StencilOp::Keep;

  StencilOp depthFailOp = StencilOp::Keep;

  CompareOp compareOp = CompareOp::Always;

  u32 compareMask = 0;

  u32 writeMask = 0;

  u32 reference = 0;
};

struct PipelineDepthStencil {
  bool depthTest = true;

  bool depthWrite = true;

  bool stencilTest = false;

  PipelineStencil front{};

  PipelineStencil back{};
};

struct PipelineMultisample {
  u32 sampleCount = 1;
};

struct GraphicsPipelineDescription {
  ShaderHandle vertexShader = ShaderHandle::Null;

  ShaderHandle fragmentShader = ShaderHandle::Null;

  PipelineVertexInputLayout inputLayout;

  PipelineInputAssembly inputAssembly;

  PipelineRasterizer rasterizer;

  PipelineColorBlend colorBlend;

  PipelineDepthStencil depthStencil;

  PipelineMultisample multisample;

  String debugName;

  RenderPassHandle renderPass = RenderPassHandle::Null;
};

struct ComputePipelineDescription {
  ShaderHandle computeShader = ShaderHandle::Null;

  String debugName;
};

} // namespace quoll::rhi
