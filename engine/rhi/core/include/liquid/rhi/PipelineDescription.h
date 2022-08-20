#pragma once

#include "RenderHandle.h"
#include "liquid/scene/Vertex.h"
#include "liquid/scene/SkinnedVertex.h"

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
  uint32_t format = 0;

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

  // TODO: Create abstract format type
  const uint32_t R32G32B32A32_SFLOAT = 109;
  const uint32_t R32G32B32_SFLOAT = 106;
  const uint32_t R32G32_SFLOAT = 103;

  return PipelineVertexInputLayout{
      {PipelineVertexInputBinding{0, sizeof(Vertex), VertexInputRate::Vertex}},
      {
          PipelineVertexInputAttribute{PositionSlot, 0, R32G32B32_SFLOAT,
                                       offsetof(SkinnedVertex, x)},
          PipelineVertexInputAttribute{NormalSlot, 0, R32G32B32_SFLOAT,
                                       offsetof(SkinnedVertex, nx)},
          PipelineVertexInputAttribute{TangentSlot, 0, R32G32B32A32_SFLOAT,
                                       offsetof(SkinnedVertex, tx)},
          PipelineVertexInputAttribute{ColorSlot, 0, R32G32B32_SFLOAT,
                                       offsetof(SkinnedVertex, r)},
          PipelineVertexInputAttribute{TexCoord0Slot, 0, R32G32_SFLOAT,
                                       offsetof(SkinnedVertex, u0)},
          PipelineVertexInputAttribute{TexCoord1Slot, 0, R32G32_SFLOAT,
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

  // TODO: Create abstract format type
  static constexpr uint32_t R32G32B32A32_SFLOAT = 109;
  static constexpr uint32_t R32G32B32_SFLOAT = 106;
  static constexpr uint32_t R32G32_SFLOAT = 103;
  static constexpr uint32_t R32G32B32A32_UINT = 107;

  return PipelineVertexInputLayout{
      {PipelineVertexInputBinding{0, sizeof(SkinnedVertex),
                                  VertexInputRate::Vertex}},
      {PipelineVertexInputAttribute{PositionSlot, 0, R32G32B32_SFLOAT,
                                    offsetof(SkinnedVertex, x)},
       PipelineVertexInputAttribute{NormalSlot, 0, R32G32B32_SFLOAT,
                                    offsetof(SkinnedVertex, nx)},
       PipelineVertexInputAttribute{TangentSlot, 0, R32G32B32A32_SFLOAT,
                                    offsetof(SkinnedVertex, tx)},
       PipelineVertexInputAttribute{ColorSlot, 0, R32G32B32_SFLOAT,
                                    offsetof(SkinnedVertex, r)},
       PipelineVertexInputAttribute{TexCoord0Slot, 0, R32G32_SFLOAT,
                                    offsetof(SkinnedVertex, u0)},
       PipelineVertexInputAttribute{TexCoord1Slot, 0, R32G32_SFLOAT,
                                    offsetof(SkinnedVertex, u1)},
       PipelineVertexInputAttribute{JointsSlot, 0, R32G32B32A32_UINT,
                                    offsetof(SkinnedVertex, j0)},
       PipelineVertexInputAttribute{WeightsSlot, 0, R32G32B32A32_SFLOAT,
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
 * @brief Pipeline description
 */
struct PipelineDescription {
  /**
   * Vertex shader
   */
  ShaderHandle vertexShader = ShaderHandle::Invalid;

  /**
   * Fragment shader
   */
  ShaderHandle fragmentShader = ShaderHandle::Invalid;

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
   * Render pass
   */
  RenderPassHandle renderPass = RenderPassHandle::Invalid;
};

} // namespace liquid::rhi
