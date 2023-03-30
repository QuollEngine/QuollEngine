#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/rhi/Descriptor.h"
#include "liquid/rhi/RenderDevice.h"
#include "Presenter.h"

namespace liquid {

Presenter::Presenter(ShaderLibrary &shaderLibrary, RenderStorage &renderStorage)
    : mShaderLibrary(shaderLibrary), mRenderStorage(renderStorage) {
  mShaderLibrary.addShader(
      "__engine.fullscreenQuad.default.vertex",
      renderStorage.getDevice()->createShader(
          {Engine::getShadersPath() / "fullscreen-quad.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.fullscreenQuad.default.fragment",
      renderStorage.getDevice()->createShader(
          {Engine::getShadersPath() / "fullscreen-quad.frag.spv"}));

  rhi::DescriptorLayoutDescription desc{};
  desc.bindings.resize(1);
  desc.bindings.at(0).binding = 0;
  desc.bindings.at(0).descriptorCount = 1;
  desc.bindings.at(0).shaderStage = rhi::ShaderStage::Fragment;
  desc.bindings.at(0).descriptorType =
      rhi::DescriptorType::CombinedImageSampler;
  desc.bindings.at(0).name = "uTexture";

  auto layout = renderStorage.getDevice()->createDescriptorLayout(desc);

  mPresentDescriptor = renderStorage.getDevice()->createDescriptor(layout);

  auto vertexShader =
      mShaderLibrary.getShader("__engine.fullscreenQuad.default.vertex");
  auto fragmentShader =
      mShaderLibrary.getShader("__engine.fullscreenQuad.default.fragment");

  rhi::GraphicsPipelineDescription pipelineDescription{};
  pipelineDescription.vertexShader = vertexShader;
  pipelineDescription.fragmentShader = fragmentShader;
  pipelineDescription.rasterizer = rhi::PipelineRasterizer{
      liquid::rhi::PolygonMode::Fill, liquid::rhi::CullMode::Front,
      liquid::rhi::FrontFace::CounterClockwise};
  pipelineDescription.colorBlend.attachments = {
      liquid::rhi::PipelineColorBlendAttachment{}};

  mPresentPipeline = mRenderStorage.addPipeline(pipelineDescription);
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
  renderPassDescription.colorAttachments.push_back(attachment);

  if (mPresentPass != rhi::RenderPassHandle::Invalid) {
    mRenderStorage.getDevice()->destroyRenderPass(mPresentPass);
  }

  mPresentPass =
      mRenderStorage.getDevice()->createRenderPass(renderPassDescription);

  auto vertexShader =
      mShaderLibrary.getShader("__engine.fullscreenQuad.default.vertex");
  auto fragmentShader =
      mShaderLibrary.getShader("__engine.fullscreenQuad.default.fragment");

  mRenderStorage.getGraphicsPipelineDescription(mPresentPipeline).renderPass =
      mPresentPass;

  auto *device = mRenderStorage.getDevice();

  if (device->hasPipeline(mPresentPipeline)) {
    device->destroyPipeline(mPresentPipeline);
  }

  device->createPipeline(
      mRenderStorage.getGraphicsPipelineDescription(mPresentPipeline),
      mPresentPipeline);

  for (auto fb : mFramebuffers) {
    device->destroyFramebuffer(fb);
  }

  mFramebuffers.resize(swapchain.textures.size());

  for (size_t i = 0; i < mFramebuffers.size(); ++i) {
    rhi::FramebufferDescription framebufferDescription{};
    framebufferDescription.width = mExtent.x;
    framebufferDescription.height = mExtent.y;
    framebufferDescription.layers = 1;
    framebufferDescription.renderPass = mPresentPass;
    framebufferDescription.attachments = {swapchain.textures.at(i)};

    mFramebuffers.at(i) = device->createFramebuffer(framebufferDescription);
  }

  if (rhi::isHandleValid(mPresentTexture)) {
    std::array<rhi::TextureHandle, 1> textures{mPresentTexture};
    mPresentDescriptor.write(0, textures,
                             rhi::DescriptorType::CombinedImageSampler);
  }
}

void Presenter::present(rhi::RenderCommandList &commandList,
                        rhi::TextureHandle handle, uint32_t imageIndex) {

  if (handle != mPresentTexture) {
    mPresentTexture = handle;
    std::array<rhi::TextureHandle, 1> textures{mPresentTexture};
    mPresentDescriptor.write(0, textures,
                             rhi::DescriptorType::CombinedImageSampler);
  }

  {
    rhi::ImageBarrier imageBarrier;
    imageBarrier.srcLayout = rhi::ImageLayout::ColorAttachmentOptimal;
    imageBarrier.dstLayout = rhi::ImageLayout::ShaderReadOnlyOptimal;
    imageBarrier.srcAccess = rhi::Access::ColorAttachmentWrite;
    imageBarrier.dstAccess = rhi::Access::ShaderRead;
    imageBarrier.texture = handle;

    std::array<rhi::ImageBarrier, 1> barriers{imageBarrier};
    commandList.pipelineBarrier(rhi::PipelineStage::ColorAttachmentOutput,
                                rhi::PipelineStage::FragmentShader, {},
                                barriers);
  }

  commandList.beginRenderPass(
      mPresentPass, mFramebuffers.at(imageIndex % mFramebuffers.size()), {0, 0},
      mExtent);

  commandList.setViewport({0.0f, 0.0f}, mExtent, {0.0f, 1.0f});
  commandList.setScissor({0.0f, 0.0f}, mExtent);

  commandList.bindPipeline(mPresentPipeline);

  commandList.bindDescriptor(mPresentPipeline, 0, mPresentDescriptor);

  commandList.draw(3, 0);

  commandList.endRenderPass();

  {
    rhi::ImageBarrier imageBarrier;
    imageBarrier.srcLayout = rhi::ImageLayout::ShaderReadOnlyOptimal;
    imageBarrier.dstLayout = rhi::ImageLayout::ColorAttachmentOptimal;
    imageBarrier.texture = handle;
    imageBarrier.srcAccess = rhi::Access::ShaderRead;
    imageBarrier.dstAccess = rhi::Access::ColorAttachmentWrite;

    std::array<rhi::ImageBarrier, 1> barriers{imageBarrier};
    commandList.pipelineBarrier(rhi::PipelineStage::FragmentShader,
                                rhi::PipelineStage::ColorAttachmentOutput, {},
                                barriers);
  }
}

} // namespace liquid
