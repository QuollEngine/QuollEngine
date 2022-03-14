#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

#include "RenderGraphEvaluator.h"
#include "RenderGraphPassResult.h"

namespace liquid {

RenderGraphEvaluator::RenderGraphEvaluator(rhi::ResourceRegistry &registry)
    : mRegistry(registry) {}

std::vector<RenderGraphPassBase *>
RenderGraphEvaluator::compile(RenderGraph &graph, bool swapchainRecreated,
                              const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::compile");

  const auto &&compiled = graph.compile();

  for (const auto &[resourceId, textureDesc] : graph.getTextures()) {
    if (!graph.getResourceRegistry().hasTexture(resourceId) ||
        (swapchainRecreated &&
         textureDesc.sizeMethod == AttachmentSizeMethod::SwapchainRelative)) {
      graph.getResourceRegistry().addTexture(
          resourceId, createTexture(textureDesc, extent));
    }
  }

  return compiled;
}

void RenderGraphEvaluator::build(std::vector<RenderGraphPassBase *> &compiled,
                                 RenderGraph &graph, bool swapchainRecreated,
                                 uint32_t numSwapchainImages,
                                 const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::build");

  for (auto &pass : compiled) {
    buildPass(pass, graph, swapchainRecreated && pass->isSwapchainRelative(),
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

    if (!force &&
        graph.getResourceRegistry().hasRenderPass(pass->getRenderPass()) &&
        (graph.getResourceRegistry().hasTexture(resourceId) ||
         graph.isSwapchain(resourceId))) {
      continue;
    }

    if (graph.isSwapchain(resourceId)) {
      info = createSwapchainAttachment(graphAttachment, numSwapchainImages,
                                       extent);
      renderPassDesc.colorAttachments.push_back(info.attachment);
    } else {
      const auto &texture = graph.getResourceRegistry().getTexture(resourceId);
      const auto &textureType = graph.getTextures().at(resourceId).type;
      if (textureType == AttachmentType::Color) {
        info = createColorAttachment(graphAttachment, texture);
        renderPassDesc.colorAttachments.push_back(info.attachment);
      } else if (textureType == AttachmentType::Depth) {
        info = createDepthAttachment(graphAttachment, texture);
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

      mRegistry.updateRenderPass(renderPass, renderPassDesc);
    } else {
      renderPass = mRegistry.addRenderPass(renderPassDesc);
    }

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
        mRegistry.updateFramebuffer(framebuffers.at(i), framebufferDesc);
      } else {
        framebuffers.at(i) = mRegistry.addFramebuffer(framebufferDesc);
      }
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
        mRegistry.updatePipeline(handle, description);
      } else {
        handle = mRegistry.addPipeline(description);
      }

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
    const RenderPassAttachment &attachment, rhi::TextureHandle texture) {
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

  // Dimensions
  info.width = desc.width;
  info.height = desc.height;
  info.layers = desc.layers;

  return info;
}

RenderGraphEvaluator::VulkanAttachmentInfo
RenderGraphEvaluator::createDepthAttachment(
    const RenderPassAttachment &attachment, rhi::TextureHandle texture) {
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

  // Dimensions
  info.width = desc.width;
  info.height = desc.height;
  info.layers = desc.layers;

  return info;
}

rhi::TextureHandle
RenderGraphEvaluator::createTexture(const AttachmentData &data,
                                    const glm::uvec2 &extent) {
  LIQUID_PROFILE_EVENT("RenderGraphEvaluator::createTexture");
  uint32_t width = data.width;
  uint32_t height = data.height;

  if (data.sizeMethod == AttachmentSizeMethod::SwapchainRelative) {
    constexpr uint32_t PERCENTAGE_RATIO = 100;
    width = extent.x * width / PERCENTAGE_RATIO;
    height = extent.y * height / PERCENTAGE_RATIO;
  }

  rhi::TextureDescription description;
  description.width = width;
  description.height = height;
  description.layers = data.layers;
  description.format = data.format;
  if (data.type == AttachmentType::Color) {
    description.usageFlags =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    description.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
  } else if (data.type == AttachmentType::Depth) {
    description.usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                             VK_IMAGE_USAGE_SAMPLED_BIT;
    description.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
  }

  return mRegistry.addTexture(description);
}

} // namespace liquid
