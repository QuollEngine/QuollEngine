#include "core/Base.h"
#include "VulkanMapping.h"

namespace liquid {

VkPrimitiveTopology
VulkanMapping::getPrimitiveTopology(PrimitiveTopology topology) {
  switch (topology) {
  case PrimitiveTopology::PointList:
    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
  case PrimitiveTopology::LineList:
    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  case PrimitiveTopology::LineStrip:
    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
  case PrimitiveTopology::TriangleList:
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  case PrimitiveTopology::TriangleStrip:
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
  case PrimitiveTopology::TriangleFan:
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
  case PrimitiveTopology::ListListWithAdjacency:
    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
  case PrimitiveTopology::LineStripWithAdjacency:
    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
  case PrimitiveTopology::TriangleListWithAdjacency:
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
  case PrimitiveTopology::TriangleStripWithAdjacency:
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
  case PrimitiveTopology::PatchList:
    return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
  default:
    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
  }
}

VkAttachmentLoadOp VulkanMapping::getAttachmentLoadOp(AttachmentLoadOp loadOp) {
  switch (loadOp) {
  case AttachmentLoadOp::DontCare:
    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  case AttachmentLoadOp::Clear:
    return VK_ATTACHMENT_LOAD_OP_CLEAR;
  case AttachmentLoadOp::Load:
    return VK_ATTACHMENT_LOAD_OP_LOAD;
  default:
    return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
  }
}

VkAttachmentStoreOp
VulkanMapping::getAttachmentStoreOp(AttachmentStoreOp storeOp) {
  switch (storeOp) {
  case AttachmentStoreOp::DontCare:
    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
  case AttachmentStoreOp::Store:
    return VK_ATTACHMENT_STORE_OP_STORE;
  default:
    return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
  }
}

VkPolygonMode VulkanMapping::getPolygonMode(PolygonMode polygonMode) {
  switch (polygonMode) {
  case PolygonMode::Fill:
    return VK_POLYGON_MODE_FILL;
  case PolygonMode::Line:
    return VK_POLYGON_MODE_LINE;
  case PolygonMode::Point:
    return VK_POLYGON_MODE_POINT;
  default:
    return VK_POLYGON_MODE_MAX_ENUM;
  }
}

VkCullModeFlags VulkanMapping::getCullMode(CullMode cullMode) {
  switch (cullMode) {
  case CullMode::Back:
    return VK_CULL_MODE_BACK_BIT;
  case CullMode::Front:
    return VK_CULL_MODE_FRONT_BIT;
  case CullMode::FrontAndBack:
    return VK_CULL_MODE_FRONT_AND_BACK;
  case CullMode::None:
    return VK_CULL_MODE_NONE;
  default:
    return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
  }
}

VkFrontFace VulkanMapping::getFrontFace(FrontFace frontFace) {
  switch (frontFace) {
  case FrontFace::Clockwise:
    return VK_FRONT_FACE_CLOCKWISE;
  case FrontFace::CounterClockwise:
    return VK_FRONT_FACE_COUNTER_CLOCKWISE;
  default:
    return VK_FRONT_FACE_MAX_ENUM;
  }
}

VkBlendFactor VulkanMapping::getBlendFactor(BlendFactor blendFactor) {
  switch (blendFactor) {
  case BlendFactor::Zero:
    return VK_BLEND_FACTOR_ZERO;
  case BlendFactor::One:
    return VK_BLEND_FACTOR_ONE;
  case BlendFactor::SrcColor:
    return VK_BLEND_FACTOR_SRC_COLOR;
  case BlendFactor::OneMinusSrcColor:
    return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
  case BlendFactor::DstColor:
    return VK_BLEND_FACTOR_DST_COLOR;
  case BlendFactor::OneMinusDstColor:
    return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
  case BlendFactor::SrcAlpha:
    return VK_BLEND_FACTOR_SRC_ALPHA;
  case BlendFactor::OneMinusSrcAlpha:
    return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  case BlendFactor::DstAlpha:
    return VK_BLEND_FACTOR_DST_ALPHA;
  case BlendFactor::OneMinusDstAlpha:
    return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
  case BlendFactor::SrcAlphaSaturate:
    return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
  case BlendFactor::Src1Color:
    return VK_BLEND_FACTOR_SRC1_COLOR;
  case BlendFactor::OneMinusSrc1Color:
    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
  case BlendFactor::Src1Alpha:
    return VK_BLEND_FACTOR_SRC1_ALPHA;
  case BlendFactor::OneMinusSrc1Alpha:
    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
  default:
    return VK_BLEND_FACTOR_MAX_ENUM;
  }
}

VkBlendOp VulkanMapping::getBlendOp(BlendOp blendOp) {
  switch (blendOp) {
  case BlendOp::Add:
    return VK_BLEND_OP_ADD;
  case BlendOp::Subtract:
    return VK_BLEND_OP_SUBTRACT;
  case BlendOp::ReverseSubtract:
    return VK_BLEND_OP_REVERSE_SUBTRACT;
  case BlendOp::Min:
    return VK_BLEND_OP_MIN;
  case BlendOp::Max:
    return VK_BLEND_OP_MAX;
  default:
    return VK_BLEND_OP_MAX_ENUM;
  }
}

VkVertexInputRate VulkanMapping::getVertexInputRate(VertexInputRate inputRate) {
  switch (inputRate) {
  case VertexInputRate::Vertex:
    return VK_VERTEX_INPUT_RATE_VERTEX;
  case VertexInputRate::Instance:
    return VK_VERTEX_INPUT_RATE_INSTANCE;
  default:
    return VK_VERTEX_INPUT_RATE_MAX_ENUM;
  }
}

} // namespace liquid
