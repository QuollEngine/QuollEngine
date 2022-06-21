#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

#include "RenderGraphEvaluator.h"

namespace liquid::rhi {

RenderGraphEvaluator::RenderGraphEvaluator(ResourceRegistry &registry)
    : mRegistry(registry) {}

void RenderGraphEvaluator::build(RenderGraph &graph) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::build");

  for (size_t index = 0; index < graph.getCompiledPasses().size(); ++index) {
    auto &pass = graph.getCompiledPasses().at(index);
    buildPass(index, graph,
              graph.isDirty() && hasSwapchainRelativeResources(pass));
  }

  graph.updateDirtyFlag();
}

void RenderGraphEvaluator::execute(RenderCommandList &commandList,
                                   RenderGraph &graph) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::execute");

  for (auto &pass : graph.getCompiledPasses()) {
    if (pass.mPreBarrier.enabled) {
      commandList.pipelineBarrier(
          pass.mPreBarrier.srcStage, pass.mPreBarrier.dstStage,
          pass.mPreBarrier.memoryBarriers, pass.mPreBarrier.imageBarriers);
    }

    commandList.beginRenderPass(pass.mRenderPass, pass.getFramebuffer(), {0, 0},
                                glm::uvec2(pass.getDimensions()));
    commandList.setViewport({0.0f, 0.0f}, glm::uvec2(pass.getDimensions()),
                            {0.0f, 1.0f});
    commandList.setScissor({0.0f, 0.0f}, glm::uvec2(pass.getDimensions()));
    pass.execute(commandList);
    commandList.endRenderPass();

    if (pass.mPostBarrier.enabled) {
      commandList.pipelineBarrier(
          pass.mPostBarrier.srcStage, pass.mPostBarrier.dstStage,
          pass.mPostBarrier.memoryBarriers, pass.mPostBarrier.imageBarriers);
    }
  }
}

void RenderGraphEvaluator::buildPass(size_t index, RenderGraph &graph,
                                     bool force) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::buildPass");
  auto &pass = graph.getCompiledPasses().at(index);

  if (!force && isHandleValid(pass.mRenderPass)) {
    return;
  }

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;

  std::vector<TextureHandle> framebufferAttachments;

  RenderPassDescription renderPassDesc{};

  for (size_t i = 0; i < pass.getOutputs().size(); ++i) {
    auto &output = pass.mOutputs.at(i);
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
    renderPassDesc.bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    bool renderPassExists = isHandleValid(pass.mRenderPass);

    pass.mRenderPass =
        mRegistry.setRenderPass(renderPassDesc, pass.mRenderPass);

    std::vector<FramebufferHandle> framebuffers(framebufferAttachments.size(),
                                                FramebufferHandle::Invalid);

    FramebufferDescription framebufferDesc;
    framebufferDesc.width = width;
    framebufferDesc.height = height;
    framebufferDesc.layers = layers;
    framebufferDesc.attachments = framebufferAttachments;
    framebufferDesc.renderPass = pass.mRenderPass;

    pass.mFramebuffer =
        mRegistry.setFramebuffer(framebufferDesc, pass.mFramebuffer);

    pass.mDimensions.x = width;
    pass.mDimensions.y = height;
    pass.mDimensions.z = layers;
  }

  LIQUID_ASSERT(isHandleValid(pass.mRenderPass), "Render pass is not created");

  for (auto resource : pass.getPipelines()) {
    auto description = mRegistry.getPipelineMap().getDescription(resource);
    description.renderPass = pass.mRenderPass;
    mRegistry.setPipeline(description, resource);
  }
}

RenderGraphEvaluator::RenderPassAttachmentInfo
RenderGraphEvaluator::createAttachment(const AttachmentData &attachment,
                                       RenderTargetData &renderTarget,
                                       const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::createAttachment");
  RenderPassAttachmentInfo info{};
  const auto &desc =
      mRegistry.getTextureMap().getDescription(renderTarget.texture);

  info.framebufferAttachments.push_back(renderTarget.texture);

  info.attachment.loadOp = attachment.loadOp;
  info.attachment.storeOp = attachment.storeOp;
  info.attachment.texture = renderTarget.texture;
  info.attachment.clearValue = attachment.clearValue;
  info.attachment.initialLayout = renderTarget.srcLayout;
  info.attachment.layout = renderTarget.dstLayout;

  constexpr uint32_t HUNDRED_PERCENT = 100;
  if (desc.sizeMethod == TextureSizeMethod::FramebufferRatio) {
    info.width = desc.width * extent.x / HUNDRED_PERCENT;
    info.height = desc.height * extent.y / HUNDRED_PERCENT;
  } else {
    info.width = desc.width;
    info.height = desc.height;
  }

  info.layers = desc.layers;

  return info;
}

bool RenderGraphEvaluator::hasSwapchainRelativeResources(
    RenderGraphPass &pass) {
  for (auto rt : pass.getOutputs()) {
    auto handle = rt.texture;
    if (handle == TextureHandle(1) ||
        mRegistry.getTextureMap().getDescription(handle).sizeMethod ==
            TextureSizeMethod::FramebufferRatio) {
      return true;
    }
  }

  return false;
}

} // namespace liquid::rhi
