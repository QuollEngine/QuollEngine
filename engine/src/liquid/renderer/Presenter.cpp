#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/core/Engine.h"
#include "liquid/rhi/Descriptor.h"
#include "liquid/rhi/RenderDevice.h"
#include "Presenter.h"

namespace liquid {

Presenter::Presenter(ShaderLibrary &shaderLibrary,
                     rhi::ResourceRegistry &registry, rhi::RenderDevice *device)
    : mRegistry(registry), mShaderLibrary(shaderLibrary), mDevice(device) {
  mShaderLibrary.addShader("__engine.fullscreenQuad.default.vertex",
                           mDevice->createShader({Engine::getShadersPath() /
                                                  "fullscreenQuad.vert.spv"}));
  mShaderLibrary.addShader("__engine.fullscreenQuad.default.fragment",
                           mDevice->createShader({Engine::getShadersPath() /
                                                  "fullscreenQuad.frag.spv"}));
}

void Presenter::updateFramebuffers(const rhi::Swapchain &swapchain) {
  mExtent = swapchain.extent;

  rhi::RenderPassAttachmentDescription attachment{};
  attachment.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  attachment.loadOp = rhi::AttachmentLoadOp::Clear;
  attachment.storeOp = rhi::AttachmentStoreOp::Store;
  attachment.texture = swapchain.textures.at(0);
  attachment.clearValue = rhi::AttachmentClearValue(glm::vec4{0.0f});

  rhi::RenderPassDescription renderPassDescription{};
  renderPassDescription.bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  renderPassDescription.attachments.push_back(attachment);

  if (mPresentPass != rhi::RenderPassHandle::Invalid) {
    mDevice->destroyRenderPass(mPresentPass);
  }

  mPresentPass = mDevice->createRenderPass(renderPassDescription);

  LOG_DEBUG("Present pass created");

  auto vertexShader =
      mShaderLibrary.getShader("__engine.fullscreenQuad.default.vertex");
  auto fragmentShader =
      mShaderLibrary.getShader("__engine.fullscreenQuad.default.fragment");

  rhi::PipelineDescription pipelineDescription{};
  pipelineDescription.vertexShader = vertexShader;
  pipelineDescription.fragmentShader = fragmentShader;
  pipelineDescription.renderPass = mPresentPass;
  pipelineDescription.rasterizer = rhi::PipelineRasterizer{
      liquid::rhi::PolygonMode::Fill, liquid::rhi::CullMode::Front,
      liquid::rhi::FrontFace::CounterClockwise};
  pipelineDescription.colorBlend.attachments = {
      liquid::rhi::PipelineColorBlendAttachment{}};

  mPresentPipeline =
      mRegistry.setPipeline(pipelineDescription, mPresentPipeline);

  LOG_DEBUG("Present pipeline created");

  for (auto fb : mFramebuffers) {
    mDevice->destroyFramebuffer(fb);
  }

  mFramebuffers.resize(swapchain.textures.size());

  for (size_t i = 0; i < mFramebuffers.size(); ++i) {
    rhi::FramebufferDescription framebufferDescription{};
    framebufferDescription.width = mExtent.x;
    framebufferDescription.height = mExtent.y;
    framebufferDescription.layers = 1;
    framebufferDescription.renderPass = mPresentPass;
    framebufferDescription.attachments = {swapchain.textures.at(i)};

    mFramebuffers.at(i) = mDevice->createFramebuffer(framebufferDescription);
  }

  LOG_DEBUG("Present framebuffers created");
}

void Presenter::present(rhi::RenderCommandList &commandList,
                        rhi::TextureHandle handle, uint32_t imageIndex) {

  {
    rhi::ImageBarrier imageBarrier;
    imageBarrier.srcLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageBarrier.dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrier.srcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageBarrier.dstAccess = VK_ACCESS_SHADER_READ_BIT;
    imageBarrier.texture = handle;

    commandList.pipelineBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, {},
                                {imageBarrier});
  }

  commandList.beginRenderPass(
      mPresentPass, mFramebuffers.at(imageIndex % mFramebuffers.size()), {0, 0},
      mExtent);

  commandList.setViewport({0.0f, 0.0f}, mExtent, {0.0f, 1.0f});
  commandList.setScissor({0.0f, 0.0f}, mExtent);

  commandList.bindPipeline(mPresentPipeline);

  rhi::Descriptor descriptor;
  descriptor.bind(0, {handle}, rhi::DescriptorType::CombinedImageSampler);
  commandList.bindDescriptor(mPresentPipeline, 0, descriptor);

  commandList.draw(3, 0);

  commandList.endRenderPass();

  {
    rhi::ImageBarrier imageBarrier;
    imageBarrier.srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrier.dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageBarrier.texture = handle;
    imageBarrier.srcAccess = VK_ACCESS_SHADER_READ_BIT;
    imageBarrier.dstAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    commandList.pipelineBarrier(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                {}, {imageBarrier});
  }
}

} // namespace liquid
