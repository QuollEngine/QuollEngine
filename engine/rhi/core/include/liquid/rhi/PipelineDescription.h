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
  const uint32_t POSITION_LOCATION = 0;
  const uint32_t NORMAL_LOCATION = 1;
  const uint32_t TANGENT_LOCATION = 2;
  const uint32_t COLOR_LOCATION = 3;
  const uint32_t TEXCOORD0_LOCATION = 4;
  const uint32_t TEXCOORD1_LOCATION = 5;

  // TODO: Create abstract format type
  const uint32_t R32G32B32A32_SFLOAT = 109;
  const uint32_t R32G32B32_SFLOAT = 106;
  const uint32_t R32G32_SFLOAT = 103;

  return PipelineVertexInputLayout{
      {PipelineVertexInputBinding{0, sizeof(Vertex), VertexInputRate::Vertex}},
      {PipelineVertexInputAttribute{POSITION_LOCATION, 0, R32G32B32_SFLOAT,
                                    offsetof(Vertex, x)},
       PipelineVertexInputAttribute{NORMAL_LOCATION, 0, R32G32B32_SFLOAT,
                                    offsetof(Vertex, nx)},
       PipelineVertexInputAttribute{TANGENT_LOCATION, 0, R32G32B32A32_SFLOAT,
                                    offsetof(Vertex, tx)},
       PipelineVertexInputAttribute{COLOR_LOCATION, 0, R32G32B32_SFLOAT,
                                    offsetof(Vertex, r)},
       PipelineVertexInputAttribute{TEXCOORD0_LOCATION, 0, R32G32_SFLOAT,
                                    offsetof(Vertex, u0)},
       PipelineVertexInputAttribute{TEXCOORD1_LOCATION, 0, R32G32_SFLOAT,
                                    offsetof(Vertex, u1)}}};
}

/**
 * @brief Create vertex input layout for Skinned vertex
 *
 * @return Pipeline vertex input layout
 */
template <>
inline PipelineVertexInputLayout
PipelineVertexInputLayout::create<SkinnedVertex>() {
  constexpr uint32_t POSITION_LOCATION = 0;
  constexpr uint32_t NORMAL_LOCATION = 1;
  constexpr uint32_t TANGENT_LOCATION = 2;
  constexpr uint32_t COLOR_LOCATION = 3;
  constexpr uint32_t TEXCOORD0_LOCATION = 4;
  constexpr uint32_t TEXCOORD1_LOCATION = 5;
  constexpr uint32_t JOINTS_LOCATION = 6;
  constexpr uint32_t WEIGHTS_LOCATION = 7;

  // TODO: Create abstract format type
  constexpr uint32_t R32G32B32A32_SFLOAT = 109;
  constexpr uint32_t R32G32B32_SFLOAT = 106;
  constexpr uint32_t R32G32_SFLOAT = 103;
  constexpr uint32_t R32G32B32A32_UINT = 107;

  return PipelineVertexInputLayout{
      {PipelineVertexInputBinding{0, sizeof(SkinnedVertex),
                                  VertexInputRate::Vertex}},
      {PipelineVertexInputAttribute{POSITION_LOCATION, 0, R32G32B32_SFLOAT,
                                    offsetof(SkinnedVertex, x)},
       PipelineVertexInputAttribute{NORMAL_LOCATION, 0, R32G32B32_SFLOAT,
                                    offsetof(SkinnedVertex, nx)},
       PipelineVertexInputAttribute{TANGENT_LOCATION, 0, R32G32B32A32_SFLOAT,
                                    offsetof(SkinnedVertex, tx)},
       PipelineVertexInputAttribute{COLOR_LOCATION, 0, R32G32B32_SFLOAT,
                                    offsetof(SkinnedVertex, r)},
       PipelineVertexInputAttribute{TEXCOORD0_LOCATION, 0, R32G32_SFLOAT,
                                    offsetof(SkinnedVertex, u0)},
       PipelineVertexInputAttribute{TEXCOORD1_LOCATION, 0, R32G32_SFLOAT,
                                    offsetof(SkinnedVertex, u1)},
       PipelineVertexInputAttribute{JOINTS_LOCATION, 0, R32G32B32A32_UINT,
                                    offsetof(SkinnedVertex, j0)},
       PipelineVertexInputAttribute{WEIGHTS_LOCATION, 0, R32G32B32A32_SFLOAT,
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
