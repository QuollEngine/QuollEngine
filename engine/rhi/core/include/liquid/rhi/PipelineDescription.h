#pragma once

#include "RenderHandle.h"
#include "liquid/scene/Vertex.h"
#include "liquid/scene/SkinnedVertex.h"
#include "liquid/rhi/Format.h"

namespace liquid::rhi {

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

  /**
   * @brief Create vertex input layout
   *
   * Generic function that returns empty layout
   *
   * @tparam InputType Vertex type
   * @return Pipeline vertex input layout
   */
  template <class InputType> static PipelineVertexInputLayout create() {
    return PipelineVertexInputLayout{};
  };
};

/**
 * @brief Create vertex input layout for Vertex
 *
 * @return Pipeline vertex input layout
 */
template <>
inline PipelineVertexInputLayout PipelineVertexInputLayout::create<Vertex>() {
  static constexpr uint32_t PositionSlot = 0;
  static constexpr uint32_t NormalSlot = 1;
  static constexpr uint32_t TangentSlot = 2;
  static constexpr uint32_t ColorSlot = 3;
  static constexpr uint32_t TexCoord0Slot = 4;
  static constexpr uint32_t TexCoord1Slot = 5;

  return PipelineVertexInputLayout{
      {PipelineVertexInputBinding{0, sizeof(Vertex), VertexInputRate::Vertex}},
      {
          PipelineVertexInputAttribute{PositionSlot, 0, rhi::Format::Rgb32Float,
                                       offsetof(SkinnedVertex, x)},
          PipelineVertexInputAttribute{NormalSlot, 0, rhi::Format::Rgb32Float,
                                       offsetof(SkinnedVertex, nx)},
          PipelineVertexInputAttribute{TangentSlot, 0, rhi::Format::Rgba32Float,
                                       offsetof(SkinnedVertex, tx)},
          PipelineVertexInputAttribute{ColorSlot, 0, rhi::Format::Rgb32Float,
                                       offsetof(SkinnedVertex, r)},
          PipelineVertexInputAttribute{TexCoord0Slot, 0, rhi::Format::Rg32Float,
                                       offsetof(SkinnedVertex, u0)},
          PipelineVertexInputAttribute{TexCoord1Slot, 0, rhi::Format::Rg32Float,
                                       offsetof(SkinnedVertex, u1)},
      }};
}

/**
 * @brief Create vertex input layout for Skinned vertex
 *
 * @return Pipeline vertex input layout
 */
template <>
inline PipelineVertexInputLayout
PipelineVertexInputLayout::create<SkinnedVertex>() {
  static constexpr uint32_t PositionSlot = 0;
  static constexpr uint32_t NormalSlot = 1;
  static constexpr uint32_t TangentSlot = 2;
  static constexpr uint32_t ColorSlot = 3;
  static constexpr uint32_t TexCoord0Slot = 4;
  static constexpr uint32_t TexCoord1Slot = 5;
  static constexpr uint32_t JointsSlot = 6;
  static constexpr uint32_t WeightsSlot = 7;

  return PipelineVertexInputLayout{
      {PipelineVertexInputBinding{0, sizeof(SkinnedVertex),
                                  VertexInputRate::Vertex}},
      {PipelineVertexInputAttribute{PositionSlot, 0, rhi::Format::Rgb32Float,
                                    offsetof(SkinnedVertex, x)},
       PipelineVertexInputAttribute{NormalSlot, 0, rhi::Format::Rgb32Float,
                                    offsetof(SkinnedVertex, nx)},
       PipelineVertexInputAttribute{TangentSlot, 0, rhi::Format::Rgba32Float,
                                    offsetof(SkinnedVertex, tx)},
       PipelineVertexInputAttribute{ColorSlot, 0, rhi::Format::Rgb32Float,
                                    offsetof(SkinnedVertex, r)},
       PipelineVertexInputAttribute{TexCoord0Slot, 0, rhi::Format::Rg32Float,
                                    offsetof(SkinnedVertex, u0)},
       PipelineVertexInputAttribute{TexCoord1Slot, 0, rhi::Format::Rg32Float,
                                    offsetof(SkinnedVertex, u1)},
       PipelineVertexInputAttribute{JointsSlot, 0, rhi::Format::Rgba32Uint,
                                    offsetof(SkinnedVertex, j0)},
       PipelineVertexInputAttribute{WeightsSlot, 0, rhi::Format::Rgba32Float,
                                    offsetof(SkinnedVertex, w0)}}};
}

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

} // namespace liquid::rhi
