#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/rhi/FramebufferDescription.h"

#include "RenderGraphEvaluator.h"

namespace liquid {

RenderGraphEvaluator::RenderGraphEvaluator(rhi::RenderDevice *device)
    : mDevice(device) {}

void RenderGraphEvaluator::build(RenderGraph &graph) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::build");

  for (size_t index = 0; index < graph.getCompiledPasses().size(); ++index) {
    auto &pass = graph.getCompiledPasses().at(index);

    if (pass.getType() == RenderGraphPassType::Compute) {
      buildComputePass(index, graph, graph.isDirty());
    } else {
      buildGraphicsPass(index, graph,
                        graph.isDirty() && hasSwapchainRelativeResources(pass));
    }
  }

  graph.updateDirtyFlag();
}

void RenderGraphEvaluator::execute(rhi::RenderCommandList &commandList,
                                   RenderGraph &graph, uint32_t frameIndex) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::execute");

  for (auto &pass : graph.getCompiledPasses()) {
    if (pass.mPreBarrier.enabled) {
      commandList.pipelineBarrier(
          pass.mPreBarrier.srcStage, pass.mPreBarrier.dstStage,
          pass.mPreBarrier.memoryBarriers, pass.mPreBarrier.imageBarriers);
    }

    if (pass.getType() == RenderGraphPassType::Compute) {
      pass.execute(commandList, frameIndex);
    } else {
      commandList.beginRenderPass(pass.mRenderPass, pass.getFramebuffer(),
                                  {0, 0}, glm::uvec2(pass.getDimensions()));
      commandList.setViewport({0.0f, 0.0f}, glm::uvec2(pass.getDimensions()),
                              {0.0f, 1.0f});
      commandList.setScissor({0.0f, 0.0f}, glm::uvec2(pass.getDimensions()));
      pass.execute(commandList, frameIndex);
      commandList.endRenderPass();
    }

    if (pass.mPostBarrier.enabled) {
      commandList.pipelineBarrier(
          pass.mPostBarrier.srcStage, pass.mPostBarrier.dstStage,
          pass.mPostBarrier.memoryBarriers, pass.mPostBarrier.imageBarriers);
    }
  }
}

void RenderGraphEvaluator::buildGraphicsPass(size_t index, RenderGraph &graph,
                                             bool force) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::buildPass");
  auto &pass = graph.getCompiledPasses().at(index);

  if (!force && isHandleValid(pass.mRenderPass)) {
    return;
  }

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;

  std::vector<rhi::TextureHandle> framebufferAttachments;

  rhi::RenderPassDescription renderPassDesc{};

  for (size_t i = 0; i < pass.getTextureOutputs().size(); ++i) {
    auto &output = pass.mTextureOutputs.at(i);
    const auto &attachment = pass.getAttachments().at(i);

    const auto &info =
        createAttachment(attachment, output, graph.getFramebufferExtent());
    renderPassDesc.attachments.push_back(info.attachment);

    for (auto handle : info.framebufferAttachments) {
      framebufferAttachments.push_back(handle);
    }

    width = info.width;
    height = info.height;
    layers = info.layers;
  }

  if (!renderPassDesc.attachments.empty()) {
    renderPassDesc.bindPoint = rhi::PipelineBindPoint::Graphics;

    bool renderPassExists = isHandleValid(pass.mRenderPass);

    if (pass.mRenderPass != rhi::RenderPassHandle::Invalid) {
      mDevice->destroyRenderPass(pass.mRenderPass);
    }

    pass.mRenderPass = mDevice->createRenderPass(renderPassDesc);

    std::vector<rhi::FramebufferHandle> framebuffers(
        framebufferAttachments.size(), rhi::FramebufferHandle::Invalid);

    rhi::FramebufferDescription framebufferDesc;
    framebufferDesc.width = width;
    framebufferDesc.height = height;
    framebufferDesc.layers = layers;
    framebufferDesc.attachments = framebufferAttachments;
    framebufferDesc.renderPass = pass.mRenderPass;

    if (pass.mFramebuffer != rhi::FramebufferHandle::Invalid) {
      mDevice->destroyFramebuffer(pass.mFramebuffer);
    }

    pass.mFramebuffer = mDevice->createFramebuffer(framebufferDesc);

    pass.mDimensions.x = width;
    pass.mDimensions.y = height;
    pass.mDimensions.z = layers;
  }

  LIQUID_ASSERT(isHandleValid(pass.mRenderPass), "Render pass is not created");

  // Graphics pipelines
  for (size_t i = 0; i < pass.mRegistry.mGraphicsPipelineDescriptions.size();
       ++i) {
    auto &description = pass.mRegistry.mGraphicsPipelineDescriptions.at(i);
    description.renderPass = pass.mRenderPass;

    auto handle = pass.mRegistry.mRealGraphicsPipelines.at(i);

    if (handle != rhi::PipelineHandle::Invalid) {
      mDevice->destroyPipeline(handle);
    }

    pass.mRegistry.mRealGraphicsPipelines.at(i) =
        mDevice->createPipeline(description);
  }
}

void RenderGraphEvaluator::buildComputePass(size_t index, RenderGraph &graph,
                                            bool force) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::buildComputePass");
  auto &pass = graph.getCompiledPasses().at(index);

  if (!force && pass.mCreated) {
    return;
  }

  for (size_t i = 0; i < pass.mRegistry.mComputePipelineDescriptions.size();
       ++i) {
    auto &description = pass.mRegistry.mComputePipelineDescriptions.at(i);

    auto handle = pass.mRegistry.mRealComputePipelines.at(i);

    if (handle != rhi::PipelineHandle::Invalid) {
      mDevice->destroyPipeline(handle);
    }

    pass.mRegistry.mRealComputePipelines.at(i) =
        mDevice->createPipeline(description);
  }

  pass.mCreated = true;
}

RenderGraphEvaluator::RenderPassAttachmentInfo
RenderGraphEvaluator::createAttachment(const AttachmentData &attachment,
                                       RenderTargetData &renderTarget,
                                       const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::createAttachment");
  RenderPassAttachmentInfo info{};
  const auto &desc = mDevice->getTextureDescription(renderTarget.texture);

  info.framebufferAttachments.push_back(renderTarget.texture);

  info.attachment.loadOp = attachment.loadOp;
  info.attachment.storeOp = attachment.storeOp;
  info.attachment.texture = renderTarget.texture;
  info.attachment.clearValue = attachment.clearValue;
  info.attachment.initialLayout = renderTarget.srcLayout;
  info.attachment.layout = renderTarget.dstLayout;

  static constexpr uint32_t HundredPercent = 100;
  if (desc.sizeMethod == rhi::TextureSizeMethod::FramebufferRatio) {
    info.width = desc.width * extent.x / HundredPercent;
    info.height = desc.height * extent.y / HundredPercent;
  } else {
    info.width = desc.width;
    info.height = desc.height;
  }

  info.layers = desc.layers;

  return info;
}

bool RenderGraphEvaluator::hasSwapchainRelativeResources(
    RenderGraphPass &pass) {
  for (auto rt : pass.getTextureOutputs()) {
    auto handle = rt.texture;
    if (handle == rhi::TextureHandle(1) ||
        mDevice->getTextureDescription(handle).sizeMethod ==
            rhi::TextureSizeMethod::FramebufferRatio) {
      return true;
    }
  }

  return false;
}

} // namespace liquid
