#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/rhi/Descriptor.h"
#include "liquid/rhi/RenderDevice.h"
#include "Presenter.h"

namespace liquid {

Presenter::Presenter(ShaderLibrary &shaderLibrary, rhi::RenderDevice *device)
    : mShaderLibrary(shaderLibrary), mDevice(device) {
  mShaderLibrary.addShader("__engine.fullscreenQuad.default.vertex",
                           mDevice->createShader({Engine::getShadersPath() /
                                                  "fullscreen-quad.vert.spv"}));
  mShaderLibrary.addShader("__engine.fullscreenQuad.default.fragment",
                           mDevice->createShader({Engine::getShadersPath() /
                                                  "fullscreen-quad.frag.spv"}));
}

void Presenter::updateFramebuffers(const rhi::Swapchain &swapchain) {
  mExtent = swapchain.extent;

  rhi::RenderPassAttachmentDescription attachment{};
  attachment.layout = rhi::ImageLayout::PresentSource;
  attachment.loadOp = rhi::AttachmentLoadOp::Clear;
  attachment.storeOp = rhi::AttachmentStoreOp::Store;
  attachment.texture = swapchain.textures.at(0);
  attachment.clearValue = rhi::AttachmentClearValue(glm::vec4{0.0f});

  rhi::RenderPassDescription renderPassDescription{};
  renderPassDescription.bindPoint = rhi::PipelineBindPoint::Graphics;
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

  if (mPresentPipeline != rhi::PipelineHandle::Invalid) {
    mDevice->destroyPipeline(mPresentPipeline);
  }

  mPresentPipeline = mDevice->createPipeline(pipelineDescription);

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
    imageBarrier.srcLayout = rhi::ImageLayout::ColorAttachmentOptimal;
    imageBarrier.dstLayout = rhi::ImageLayout::ShaderReadOnlyOptimal;
    imageBarrier.srcAccess = rhi::Access::ColorAttachmentWrite;
    imageBarrier.dstAccess = rhi::Access::ShaderRead;
    imageBarrier.texture = handle;

    commandList.pipelineBarrier(rhi::PipelineStage::ColorAttachmentOutput,
                                rhi::PipelineStage::FragmentShader, {},
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
    imageBarrier.srcLayout = rhi::ImageLayout::ShaderReadOnlyOptimal;
    imageBarrier.dstLayout = rhi::ImageLayout::ColorAttachmentOptimal;
    imageBarrier.texture = handle;
    imageBarrier.srcAccess = rhi::Access::ShaderRead;
    imageBarrier.dstAccess = rhi::Access::ColorAttachmentWrite;

    commandList.pipelineBarrier(rhi::PipelineStage::FragmentShader,
                                rhi::PipelineStage::ColorAttachmentOutput, {},
                                {imageBarrier});
  }
}

} // namespace liquid
