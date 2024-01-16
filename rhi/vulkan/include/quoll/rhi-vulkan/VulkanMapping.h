#pragma once

#include "quoll/rhi/AccessFlags.h"
#include "quoll/rhi/Descriptor.h"
#include "quoll/rhi/Filter.h"
#include "quoll/rhi/Format.h"
#include "quoll/rhi/ImageLayout.h"
#include "quoll/rhi/IndexType.h"
#include "quoll/rhi/PipelineDescription.h"
#include "quoll/rhi/RenderPassDescription.h"
#include "quoll/rhi/StageFlags.h"
#include "quoll/rhi/WrapMode.h"
#include "VulkanHeaders.h"

namespace quoll::rhi {

class VulkanMapping {
public:
  static VkPrimitiveTopology getPrimitiveTopology(PrimitiveTopology topology);

  static VkAttachmentLoadOp getAttachmentLoadOp(AttachmentLoadOp loadOp);

  static VkAttachmentStoreOp getAttachmentStoreOp(AttachmentStoreOp storeOp);

  static VkPolygonMode getPolygonMode(PolygonMode polygonMode);

  static VkCullModeFlags getCullMode(CullMode cullMode);

  static VkFrontFace getFrontFace(FrontFace frontFace);

  static VkBlendFactor getBlendFactor(BlendFactor blendFactor);

  static VkBlendOp getBlendOp(BlendOp blendOp);

  static VkCompareOp getCompareOp(CompareOp compareOp);

  static VkStencilOp getStencilOp(StencilOp stencilOp);

  static VkVertexInputRate getVertexInputRate(VertexInputRate vertexInputRate);

  static VkDescriptorType getDescriptorType(DescriptorType descriptorType);

  static VkAccessFlags2 getAccessFlags(Access access);

  static VkImageLayout getImageLayout(ImageLayout imageLayout);

  static VkPipelineBindPoint
  getPipelineBindPoint(PipelineBindPoint pipelineBindPoint);

  static VkShaderStageFlags getShaderStageFlags(ShaderStage shaderStage);

  static VkPipelineStageFlags2
  getPipelineStageFlags(PipelineStage pipelineStage);

  static VkIndexType getIndexType(IndexType indexType);

  static VkFormat getFormat(Format format);

  static VkFilter getFilter(Filter filter);

  static VkSamplerAddressMode getAddressMode(WrapMode wrapMode);

  // Vulkan to RHI mapping
public:
  static DescriptorType getDescriptorType(VkDescriptorType descriptorType);

  static ShaderStage getShaderStage(VkShaderStageFlags stageFlags);
};

} // namespace quoll::rhi
