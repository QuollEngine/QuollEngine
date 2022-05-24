#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

#include "RenderGraphEvaluator.h"

namespace liquid::rhi {

RenderGraphEvaluator::RenderGraphEvaluator(ResourceRegistry &registry)
    : mRegistry(registry) {}

void RenderGraphEvaluator::build(std::vector<size_t> &sorted,
                                 RenderGraph &graph) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::build");

  for (auto pass : sorted) {
    buildPass(pass, graph,
              graph.isDirty() &&
                  hasSwapchainRelativeResources(graph.getPasses().at(pass)));
  }

  graph.updateDirtyFlag();
}

void RenderGraphEvaluator::execute(RenderCommandList &commandList,
                                   const std::vector<size_t> &sorted,
                                   RenderGraph &graph) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::execute");

  for (auto index : sorted) {
    auto &pass = graph.getPasses().at(index);
    commandList.beginRenderPass(pass.mRenderPass, pass.getFramebuffer(), {0, 0},
                                glm::uvec2(pass.getDimensions()));
    commandList.setViewport({0.0f, 0.0f}, glm::uvec2(pass.getDimensions()),
                            {0.0f, 1.0f});
    commandList.setScissor({0.0f, 0.0f}, glm::uvec2(pass.getDimensions()));

    pass.execute(commandList);
    commandList.endRenderPass();
  }
}

void RenderGraphEvaluator::buildPass(size_t index, RenderGraph &graph,
                                     bool force) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::buildPass");
  auto &pass = graph.getPasses().at(index);

  if (!force && isHandleValid(pass.mRenderPass)) {
    return;
  }

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;

  std::vector<TextureHandle> framebufferAttachments;

  RenderPassDescription renderPassDesc{};

  for (size_t i = 0; i < pass.getOutputs().size(); ++i) {
    auto resourceId = pass.getOutputs().at(i);
    const auto &attachment = pass.getAttachments().at(i);

    RenderPassAttachmentInfo info{};

    const auto &texture = mRegistry.getTextureMap().getDescription(resourceId);
    if ((texture.usage & TextureUsage::Color) == TextureUsage::Color) {
      info = createColorAttachment(attachment, resourceId,
                                   graph.getFramebufferExtent());
      renderPassDesc.attachments.push_back(info.attachment);
    } else if ((texture.usage & TextureUsage::Depth) == TextureUsage::Depth) {
      info = createDepthAttachment(attachment, resourceId,
                                   graph.getFramebufferExtent());
      renderPassDesc.attachments.push_back(info.attachment);
    }

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
RenderGraphEvaluator::createColorAttachment(const AttachmentData &attachment,
                                            TextureHandle texture,
                                            const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::createColorAttachment");
  RenderPassAttachmentInfo info{};

  const auto &desc = mRegistry.getTextureMap().getDescription(texture);

  info.framebufferAttachments.push_back(texture);

  info.attachment.loadOp = attachment.loadOp;
  info.attachment.storeOp = attachment.storeOp;
  info.attachment.texture = texture;
  info.attachment.clearValue = attachment.clearValue;

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

RenderGraphEvaluator::RenderPassAttachmentInfo
RenderGraphEvaluator::createDepthAttachment(const AttachmentData &attachment,
                                            TextureHandle texture,
                                            const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::createDepthAttachment");
  RenderPassAttachmentInfo info{};
  const auto &desc = mRegistry.getTextureMap().getDescription(texture);

  info.framebufferAttachments.push_back(texture);

  info.attachment.loadOp = attachment.loadOp;
  info.attachment.storeOp = attachment.storeOp;
  info.attachment.texture = texture;
  info.attachment.clearValue = attachment.clearValue;

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
  for (auto handle : pass.getOutputs()) {
    if (handle == TextureHandle(1) ||
        mRegistry.getTextureMap().getDescription(handle).sizeMethod ==
            TextureSizeMethod::FramebufferRatio) {
      return true;
    }
  }

  return false;
}

} // namespace liquid::rhi
