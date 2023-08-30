#pragma once

#include "RenderHandle.h"
#include "quoll/rhi/Format.h"

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

/**
 * @brief Pipeline input assembly
 */
struct PipelineInputAssembly {
  /**
   * Primitive topology
   */
  PrimitiveTopology primitiveTopology = PrimitiveTopology::TriangleList;
};

/**
 * @brief Pipeline rasterizer
 */
struct PipelineRasterizer {
  /**
   * Polygon mode
   */
  PolygonMode polygonMode = PolygonMode::Fill;

  /**
   * Cull mode
   */
  CullMode cullMode = CullMode::None;

  /**
   * Front face direction
   */
  FrontFace frontFace = FrontFace::Clockwise;

  /**
   * @brief Line width
   */
  float lineWidth = 1.0f;
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

/**
 * @brief Pipeline vertex input binding
 */
struct PipelineVertexInputBinding {
  /**
   * Vertex input binding
   */
  uint32_t binding = 0;

  /**
   * Vertex input stride
   */
  uint32_t stride = 0;

  /**
   * Vertex input rate
   */
  VertexInputRate inputRate = VertexInputRate::Vertex;
};

/**
 * @brief Pipeline vertex input attribute
 */
struct PipelineVertexInputAttribute {
  /**
   * Attribute slot
   */
  uint32_t slot = 0;

  /**
   * Attribute binding
   */
  uint32_t binding = 0;

  /**
   * Attribute format
   */
  Format format = Format::Undefined;

  /**
   * Attribute offset
   */
  uint32_t offset = 0;
};

/**
 * @brief Pipeline vertex input layout
 */
struct PipelineVertexInputLayout {
  /**
   * Input layout bindings
   */
  std::vector<PipelineVertexInputBinding> bindings;

  /**
   * Input layout attributes
   */
  std::vector<PipelineVertexInputAttribute> attributes;
};

/**
 * @brief Pipeline color blend attachment
 */
struct PipelineColorBlendAttachment {
  /**
   * Color blend attachment enabled
   */
  bool enabled = false;

  /**
   * Source color factor
   */
  BlendFactor srcColor = BlendFactor::Zero;

  /**
   * Destination color factor
   */
  BlendFactor dstColor = BlendFactor::Zero;

  /**
   * Color blend operation
   */
  BlendOp colorOp = BlendOp::Add;

  /**
   * Source alpha factor
   */
  BlendFactor srcAlpha = BlendFactor::Zero;

  /**
   * Destination alpha factor
   */
  BlendFactor dstAlpha = BlendFactor::Zero;

  /**
   * Alpha blend operation
   */
  BlendOp alphaOp = BlendOp::Add;
};

/**
 * @brief Pipeline color blending description
 */
struct PipelineColorBlend {
  /**
   * Color blend attachments
   */
  std::vector<PipelineColorBlendAttachment> attachments;
};

/**
 * @brief Pipeline stencil description
 */
struct PipelineStencil {
  /**
   * Stencil fail operation
   */
  StencilOp failOp = StencilOp::Keep;

  /**
   * Stencil and depth pass operation
   */
  StencilOp passOp = StencilOp::Keep;

  /**
   * Stencil pass but depth fail operation
   */
  StencilOp depthFailOp = StencilOp::Keep;

  /**
   * Compare operation
   */
  CompareOp compareOp = CompareOp::Always;

  /**
   * Compare mask
   */
  uint32_t compareMask = 0;

  /**
   * Write mask
   */
  uint32_t writeMask = 0;

  /**
   * Reference value
   */
  uint32_t reference = 0;
};

/**
 * @brief Pipeline depth stencil state
 */
struct PipelineDepthStencil {
  /**
   * Depth test
   */
  bool depthTest = true;

  /**
   * Depth write
   */
  bool depthWrite = true;

  /**
   * Stencil test
   */
  bool stencilTest = false;

  /**
   * Front stencil
   */
  PipelineStencil front{};

  /**
   * Back stencil
   */
  PipelineStencil back{};
};

/**
 * @brief Pipeline multisampling description
 */
struct PipelineMultisample {
  /**
   * Sample count
   */
  uint32_t sampleCount = 1;
};

/**
 * @brief Graphics pipeline description
 */
struct GraphicsPipelineDescription {
  /**
   * Vertex shader
   */
  ShaderHandle vertexShader = ShaderHandle::Null;

  /**
   * Fragment shader
   */
  ShaderHandle fragmentShader = ShaderHandle::Null;

  /**
   * Vertex input layout
   */
  PipelineVertexInputLayout inputLayout;

  /**
   * Input assembly
   */
  PipelineInputAssembly inputAssembly;

  /**
   * Rasterizer
   */
  PipelineRasterizer rasterizer;

  /**
   * Color blending
   */
  PipelineColorBlend colorBlend;

  /**
   * Depth stencil
   */
  PipelineDepthStencil depthStencil;

  /**
   * Multisampling
   */
  PipelineMultisample multisample;

  /**
   * Debug name
   */
  String debugName;

  /**
   * Render pass
   */
  RenderPassHandle renderPass = RenderPassHandle::Null;
};

/**
 * @brief Compute pipeline description
 */
struct ComputePipelineDescription {
  /**
   * Compute shader
   */
  ShaderHandle computeShader = ShaderHandle::Null;

  /**
   * Debug name
   */
  String debugName;
};

} // namespace quoll::rhi
