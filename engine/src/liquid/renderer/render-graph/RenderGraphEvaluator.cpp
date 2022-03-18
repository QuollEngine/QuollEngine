#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

#include "RenderGraphEvaluator.h"
#include "RenderGraphPassResult.h"

namespace liquid {

RenderGraphEvaluator::RenderGraphEvaluator(rhi::ResourceRegistry &registry)
    : mRegistry(registry) {}

void RenderGraphEvaluator::build(std::vector<RenderGraphPassBase *> &compiled,
                                 RenderGraph &graph, bool swapchainRecreated,
                                 uint32_t numSwapchainImages,
                                 const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::build");

  for (auto &pass : compiled) {
    buildPass(pass, graph,
              swapchainRecreated && hasSwapchainRelativeResources(pass),
              numSwapchainImages, extent);
  }
}

void RenderGraphEvaluator::execute(
    rhi::RenderCommandList &commandList,
    const std::vector<RenderGraphPassBase *> &result, RenderGraph &graph,
    uint32_t imageIdx) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::execute");

  for (auto &item : result) {
    auto &graphPass =
        graph.getResourceRegistry().getRenderPass(item->getRenderPass());

    commandList.beginRenderPass(
        graphPass.getRenderPass(),
        graphPass.getFramebuffers().at(imageIdx %
                                       graphPass.getFramebuffers().size()),
        {0, 0}, graphPass.getExtent(), graphPass.getClearValues());
    commandList.setViewport({0.0f, 0.0f}, graphPass.getExtent(), {0.0f, 1.0f});
    commandList.setScissor({0.0f, 0.0f}, graphPass.getExtent());

    item->execute(commandList, graph.getResourceRegistry());
    commandList.endRenderPass();
  }
}

void RenderGraphEvaluator::buildPass(RenderGraphPassBase *pass,
                                     RenderGraph &graph, bool force,
                                     uint32_t numSwapchainImages,
                                     const glm::uvec2 &extent) {

  if (!force &&
      graph.getResourceRegistry().hasRenderPass(pass->getRenderPass())) {
    return;
  }

  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::buildPass");
  std::vector<VkClearValue> clearValues;

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;

  size_t imageViewsPerFramebuffer = 1;

  for (auto &[resourceId, _] : pass->getOutputs()) {
    if (graph.isSwapchain(resourceId)) {
      imageViewsPerFramebuffer = numSwapchainImages;
      break;
    }
  }

  std::vector<std::vector<rhi::TextureHandle>> framebufferAttachments(
      imageViewsPerFramebuffer);

  rhi::RenderPassDescription renderPassDesc{};
  rhi::RenderPassHandle renderPass = rhi::RenderPassHandle::Invalid;

  for (auto &[resourceId, graphAttachment] : pass->getOutputs()) {
    VulkanAttachmentInfo info{};

    if (graph.isSwapchain(resourceId)) {
      info = createSwapchainAttachment(graphAttachment, numSwapchainImages,
                                       extent);
      renderPassDesc.colorAttachments.push_back(info.attachment);
    } else {

      const auto &texture =
          mRegistry.getTextureMap().getDescription(resourceId);
      if ((texture.usage & rhi::TextureUsage::Color) ==
          rhi::TextureUsage::Color) {
        info = createColorAttachment(graphAttachment, resourceId, extent);
        renderPassDesc.colorAttachments.push_back(info.attachment);
      } else if ((texture.usage & rhi::TextureUsage::Depth) ==
                 rhi::TextureUsage::Depth) {
        info = createDepthAttachment(graphAttachment, resourceId, extent);
        renderPassDesc.depthAttachment = info.attachment;
      }
    }

    clearValues.push_back(info.clearValue);

    for (size_t i = 0; i < framebufferAttachments.size(); ++i) {
      auto handle = info.framebufferAttachments.at(
          i % info.framebufferAttachments.size());

      framebufferAttachments.at(i).push_back(handle);
    }

    width = info.width;
    height = info.height;
    layers = info.layers;
  }

  if (!renderPassDesc.colorAttachments.empty() ||
      rhi::isHandleValid(renderPassDesc.depthAttachment.texture)) {
    renderPassDesc.bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    bool renderPassExists =
        graph.getResourceRegistry().hasRenderPass(pass->getRenderPass());

    if (renderPassExists) {
      renderPass = graph.getResourceRegistry()
                       .getRenderPass(pass->getRenderPass())
                       .getRenderPass();
    }

    renderPass = mRegistry.setRenderPass(renderPassDesc, renderPass);

    std::vector<rhi::FramebufferHandle> framebuffers(
        framebufferAttachments.size(), rhi::FramebufferHandle::Invalid);

    for (size_t i = 0; i < framebuffers.size(); ++i) {
      rhi::FramebufferDescription framebufferDesc;
      framebufferDesc.width = width;
      framebufferDesc.height = height;
      framebufferDesc.layers = layers;
      framebufferDesc.attachments = framebufferAttachments.at(i);
      framebufferDesc.renderPass = renderPass;

      if (renderPassExists) {
        framebuffers.at(i) = graph.getResourceRegistry()
                                 .getRenderPass(pass->getRenderPass())
                                 .getFramebuffers()
                                 .at(i);
      }
      framebuffers.at(i) =
          mRegistry.setFramebuffer(framebufferDesc, framebuffers.at(i));
    }

    if (framebuffers.size() > 0) {
      LOG_DEBUG("[Vulkan] Framebuffers created");
    }

    graph.getResourceRegistry().addRenderPass(
        pass->getRenderPass(),
        RenderGraphPassResult(renderPass, framebuffers, clearValues, width,
                              height, layers));
  }

  LIQUID_ASSERT(
      graph.getResourceRegistry().hasRenderPass(pass->getRenderPass()),
      "Render pass with ID " + std::to_string(pass->getRenderPass()) +
          " does not exist and is not created");

  for (auto resource : pass->getResources()) {
    if (graph.isPipeline(resource)) {
      bool hasPipeline = graph.getResourceRegistry().hasPipeline(resource);
      if (!force && hasPipeline) {
        continue;
      }

      const auto &desc = graph.getPipeline(resource);
      rhi::PipelineDescription description;
      description.vertexShader = desc.vertexShader;
      description.fragmentShader = desc.fragmentShader;
      description.colorBlend = desc.colorBlend;
      description.inputAssembly = desc.inputAssembly;
      description.rasterizer = desc.rasterizer;
      description.inputLayout = desc.inputLayout;
      description.renderPass = renderPass;

      rhi::PipelineHandle handle = rhi::PipelineHandle::Invalid;
      if (hasPipeline) {
        handle = graph.getResourceRegistry().getPipeline(resource);
      }

      handle = mRegistry.setPipeline(description, handle);
      graph.getResourceRegistry().addPipeline(resource, handle);
    }
  }
}

RenderGraphEvaluator::VulkanAttachmentInfo
RenderGraphEvaluator::createSwapchainAttachment(
    const RenderPassAttachment &attachment, uint32_t numSwapchainImages,
    const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::createSwapchainAttachment");
  LIQUID_ASSERT(numSwapchainImages > 0,
                "Number of swapchain images must be greater than ZERO");
  VulkanAttachmentInfo info{};

  // Attachment clear value
  info.clearValue.color.float32[0] =
      std::get<glm::vec4>(attachment.clearValue).x;
  info.clearValue.color.float32[1] =
      std::get<glm::vec4>(attachment.clearValue).y;
  info.clearValue.color.float32[2] =
      std::get<glm::vec4>(attachment.clearValue).z;
  info.clearValue.color.float32[3] =
      std::get<glm::vec4>(attachment.clearValue).w;

  // Framebuffer image views
  info.framebufferAttachments.resize(numSwapchainImages,
                                     rhi::TextureHandle::Invalid);

  for (uint32_t i = 0; i < numSwapchainImages; ++i) {
    auto handle = static_cast<rhi::TextureHandle>(i + 1);

    info.framebufferAttachments.at(i) = handle;
  }

  info.attachment.loadOp = attachment.loadOp;
  info.attachment.storeOp = attachment.storeOp;
  info.attachment.texture = rhi::TextureHandle{1};
  info.attachment.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // Dimensions
  info.width = extent.x;
  info.height = extent.y;
  info.layers = 1;

  return info;
}

RenderGraphEvaluator::VulkanAttachmentInfo
RenderGraphEvaluator::createColorAttachment(
    const RenderPassAttachment &attachment, rhi::TextureHandle texture,
    const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::createColorAttachment");
  VulkanAttachmentInfo info{};

  const auto &desc = mRegistry.getTextureMap().getDescription(texture);

  // Attachment clear value
  info.clearValue.color.float32[0] =
      std::get<glm::vec4>(attachment.clearValue).x;
  info.clearValue.color.float32[1] =
      std::get<glm::vec4>(attachment.clearValue).y;
  info.clearValue.color.float32[2] =
      std::get<glm::vec4>(attachment.clearValue).z;
  info.clearValue.color.float32[3] =
      std::get<glm::vec4>(attachment.clearValue).w;

  info.framebufferAttachments.push_back(texture);

  info.attachment.loadOp = attachment.loadOp;
  info.attachment.storeOp = attachment.storeOp;
  info.attachment.texture = texture;

  constexpr uint32_t HUNDRED_PERCENT = 100;
  if (desc.sizeMethod == rhi::TextureSizeMethod::SwapchainRatio) {
    info.width = desc.width * extent.x / HUNDRED_PERCENT;
    info.height = desc.height * extent.y / HUNDRED_PERCENT;
  } else {
    info.width = desc.width;
    info.height = desc.height;
  }

  info.layers = desc.layers;

  return info;
}

RenderGraphEvaluator::VulkanAttachmentInfo
RenderGraphEvaluator::createDepthAttachment(
    const RenderPassAttachment &attachment, rhi::TextureHandle texture,
    const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::createDepthAttachment");
  VulkanAttachmentInfo info{};

  const auto &desc = mRegistry.getTextureMap().getDescription(texture);

  // Attachment clear value
  info.clearValue.depthStencil.depth =
      std::get<DepthStencilClear>(attachment.clearValue).clearDepth;
  info.clearValue.depthStencil.stencil =
      std::get<DepthStencilClear>(attachment.clearValue).clearStencil;

  info.framebufferAttachments.push_back(texture);

  info.attachment.loadOp = attachment.loadOp;
  info.attachment.storeOp = attachment.storeOp;
  info.attachment.texture = texture;

  constexpr uint32_t HUNDRED_PERCENT = 100;
  if (desc.sizeMethod == rhi::TextureSizeMethod::SwapchainRatio) {
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
    RenderGraphPassBase *pass) {
  for (auto &[handle, _] : pass->getOutputs()) {
    if (handle == rhi::TextureHandle(1) ||
        mRegistry.getTextureMap().getDescription(handle).sizeMethod ==
            rhi::TextureSizeMethod::SwapchainRatio) {
      return true;
    }
  }

  return false;
}

} // namespace liquid
