#pragma once

#include "liquid/renderer/Shader.h"
#include "liquid/scene/Vertex.h"
#include "liquid/scene/SkinnedVertex.h"

#include <vulkan/vulkan.hpp>

namespace liquid {

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

struct PipelineVertexInputBinding {
  uint32_t binding = 0;
  uint32_t stride = 0;
  VertexInputRate inputRate = VertexInputRate::Vertex;
};

struct PipelineVertexInputAttribute {
  uint32_t slot = 0;
  uint32_t binding = 0;
  uint32_t format = 0;
  uint32_t offset = 0;
};

struct PipelineVertexInputLayout {
  std::vector<PipelineVertexInputBinding> bindings;
  std::vector<PipelineVertexInputAttribute> attributes;

  template <class InputType> static PipelineVertexInputLayout create() {
    return PipelineVertexInputLayout{};
  };
};

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

struct PipelineDescriptor {
  SharedPtr<Shader> vertexShader;
  SharedPtr<Shader> fragmentShader;
  PipelineVertexInputLayout inputLayout;
  PipelineInputAssembly inputAssembly;
  PipelineRasterizer rasterizer;
  PipelineColorBlend colorBlend;
};

} // namespace liquid
