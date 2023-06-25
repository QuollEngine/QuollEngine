#include "liquid/core/Base.h"
#include "RenderGraphSyncDependency.h"

namespace liquid {

RenderGraphTextureSyncDependency
RenderGraphSyncDependency::getTextureRead(RenderGraphPassType type) {
  if (type == RenderGraphPassType::Compute) {
    return {rhi::PipelineStage::ComputeShader, rhi::Access::ShaderRead,
            rhi::ImageLayout::ShaderReadOnlyOptimal};
  }

  return {rhi::PipelineStage::FragmentShader, rhi::Access::ShaderRead,
          rhi::ImageLayout::ShaderReadOnlyOptimal};
}

RenderGraphTextureSyncDependency
RenderGraphSyncDependency::getTextureWrite(RenderGraphPassType type,
                                           AttachmentType attachmentType) {
  if (type == RenderGraphPassType::Compute) {
    return {rhi::PipelineStage::ComputeShader, rhi::Access::ShaderWrite,
            rhi::ImageLayout::General};
  }

  if (attachmentType == AttachmentType::Color ||
      attachmentType == AttachmentType::Resolve) {
    return {rhi::PipelineStage::ColorAttachmentOutput,
            rhi::Access::ColorAttachmentWrite,
            rhi::ImageLayout::ColorAttachmentOptimal};
  }

  if (attachmentType == AttachmentType::Depth) {
    return {rhi::PipelineStage::EarlyFragmentTests |
                rhi::PipelineStage::LateFragmentTests,
            rhi::Access::DepthStencilAttachmentWrite,
            rhi::ImageLayout::DepthStencilAttachmentOptimal};
  }

  return {rhi::PipelineStage::None, rhi::Access::None,
          rhi::ImageLayout::Undefined};
}

RenderGraphBufferSyncDependency
RenderGraphSyncDependency::getBufferWrite(RenderGraphPassType type) {
  if (type == RenderGraphPassType::Compute) {
    return {rhi::PipelineStage::ComputeShader, rhi::Access::ShaderWrite};
  }

  return {rhi::PipelineStage::FragmentShader, rhi::Access::ShaderWrite};
}

RenderGraphBufferSyncDependency
RenderGraphSyncDependency::getBufferRead(RenderGraphPassType type,
                                         rhi::BufferUsage usage) {
  if (type == RenderGraphPassType::Compute) {
    return {rhi::PipelineStage::ComputeShader, rhi::Access::ShaderRead};
  }

  rhi::Access access = rhi::Access::None;
  rhi::PipelineStage stage = rhi::PipelineStage::None;

  if (BitwiseEnumContains(usage, rhi::BufferUsage::Vertex)) {
    stage |= rhi::PipelineStage::VertexInput;
    access |= rhi::Access::VertexAttributeRead;
  }

  if (BitwiseEnumContains(usage, rhi::BufferUsage::Index)) {
    stage |= rhi::PipelineStage::VertexInput;
    access |= rhi::Access::IndexRead;
  }

  if (BitwiseEnumContains(usage, rhi::BufferUsage::Indirect)) {
    stage |= rhi::PipelineStage::DrawIndirect;
    access |= rhi::Access::IndirectCommandRead;
  }

  if (BitwiseEnumContains(usage, rhi::BufferUsage::Uniform) ||
      BitwiseEnumContains(usage, rhi::BufferUsage::Storage)) {
    stage |= rhi::PipelineStage::FragmentShader;
    access |= rhi::Access::ShaderRead;
  }

  return {stage, access};
}

} // namespace liquid
