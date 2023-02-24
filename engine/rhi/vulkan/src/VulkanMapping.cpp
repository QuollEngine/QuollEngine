#include "liquid/core/Base.h"
#include "VulkanMapping.h"

namespace liquid::rhi {

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

VkDescriptorType
VulkanMapping::getDescriptorType(DescriptorType descriptorType) {
  switch (descriptorType) {
  case DescriptorType::CombinedImageSampler:
    return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  case DescriptorType::UniformBuffer:
    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  case DescriptorType::StorageBuffer:
    return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  case DescriptorType::StorageImage:
    return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  default:
    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
  }
}

VkAccessFlags VulkanMapping::getAccessFlags(Access access) {
  return static_cast<VkAccessFlags>(access);
}

VkImageLayout VulkanMapping::getImageLayout(ImageLayout imageLayout) {
  switch (imageLayout) {
  case ImageLayout::Undefined:
    return VK_IMAGE_LAYOUT_UNDEFINED;
  case ImageLayout::General:
    return VK_IMAGE_LAYOUT_GENERAL;
  case ImageLayout::ColorAttachmentOptimal:
    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  case ImageLayout::DepthStencilAttachmentOptimal:
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  case ImageLayout::DepthStencilReadOnlyOptimal:
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
  case ImageLayout::ShaderReadOnlyOptimal:
    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  case ImageLayout::TransferSourceOptimal:
    return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  case ImageLayout::TransferDestinationOptimal:
    return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  case ImageLayout::Preinitialized:
    return VK_IMAGE_LAYOUT_PREINITIALIZED;
  case ImageLayout::PresentSource:
    return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  default:
    return VK_IMAGE_LAYOUT_MAX_ENUM;
  };
}

VkPipelineBindPoint
VulkanMapping::getPipelineBindPoint(PipelineBindPoint pipelineBindPoint) {
  switch (pipelineBindPoint) {
  case PipelineBindPoint::Graphics:
    return VK_PIPELINE_BIND_POINT_GRAPHICS;
  case PipelineBindPoint::Compute:
    return VK_PIPELINE_BIND_POINT_COMPUTE;
  default:
    return VK_PIPELINE_BIND_POINT_MAX_ENUM;
  }
}

VkShaderStageFlags VulkanMapping::getShaderStageFlags(ShaderStage shaderStage) {
  return static_cast<VkShaderStageFlags>(shaderStage);
}

VkPipelineStageFlags
VulkanMapping::getPipelineStageFlags(PipelineStage pipelineStage) {
  return static_cast<VkPipelineStageFlags>(pipelineStage);
}

VkIndexType VulkanMapping::getIndexType(IndexType indexType) {
  switch (indexType) {
  case IndexType::Uint16:
    return VK_INDEX_TYPE_UINT16;
  case IndexType::Uint32:
    return VK_INDEX_TYPE_UINT32;
  default:
    return VK_INDEX_TYPE_MAX_ENUM;
  }
}

VkFormat VulkanMapping::getFormat(Format format) {
  switch (format) {
  case rhi::Format::Rgba8Unorm:
    return VK_FORMAT_R8G8B8A8_UNORM;
  case rhi::Format::Rgba8Srgb:
    return VK_FORMAT_R8G8B8A8_SRGB;
  case rhi::Format::Bgra8Srgb:
    return VK_FORMAT_B8G8R8A8_SRGB;
  case rhi::Format::Rgba16Float:
    return VK_FORMAT_R16G16B16A16_SFLOAT;
  case rhi::Format::Rg32Float:
    return VK_FORMAT_R32G32_SFLOAT;
  case rhi::Format::Rgb32Float:
    return VK_FORMAT_R32G32B32_SFLOAT;
  case rhi::Format::Rgba32Float:
    return VK_FORMAT_R32G32B32A32_SFLOAT;
  case rhi::Format::Rgba32Uint:
    return VK_FORMAT_R32G32B32A32_UINT;
  case rhi::Format::Depth16Unorm:
    return VK_FORMAT_D16_UNORM;
  case rhi::Format::Depth32Float:
    return VK_FORMAT_D32_SFLOAT;
  case rhi::Format::Undefined:
  default:
    LIQUID_ASSERT(false, "Undefined format");
    return VK_FORMAT_UNDEFINED;
  }
}

VkFilter VulkanMapping::getFilter(Filter filter) {
  switch (filter) {
  case Filter::Nearest:
    return VK_FILTER_NEAREST;
  case Filter::Linear:
    return VK_FILTER_LINEAR;
  default:
    LIQUID_ASSERT(false, "Filter does not exist");
  }
}

DescriptorType
VulkanMapping::getDescriptorType(VkDescriptorType descriptorType) {
  switch (descriptorType) {
  case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
    return DescriptorType::CombinedImageSampler;
  case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
    return DescriptorType::UniformBuffer;
  case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
    return DescriptorType::StorageBuffer;
  case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
    return DescriptorType::StorageImage;
  default:
    LIQUID_ASSERT(false, "Descriptor type does not exist");
  }
}

ShaderStage VulkanMapping::getShaderStage(VkShaderStageFlags stageFlags) {
  return static_cast<ShaderStage>(stageFlags);
}

} // namespace liquid::rhi
