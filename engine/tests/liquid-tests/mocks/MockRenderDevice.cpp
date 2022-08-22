#include "liquid/core/Base.h"
#include "MockRenderDevice.h"

liquid::rhi::RenderFrame MockRenderDevice::beginFrame() {
  return liquid::rhi::RenderFrame{std::numeric_limits<uint32_t>::max(),
                                  std::numeric_limits<uint32_t>::max(),
                                  mNoopCommandList};
}

void MockRenderDevice::endFrame(const liquid::rhi::RenderFrame &renderFrame) {}

void MockRenderDevice::waitForIdle() {}

void MockRenderDevice::synchronize(liquid::rhi::ResourceRegistry &registry) {}

const liquid::rhi::PhysicalDeviceInformation
MockRenderDevice::getDeviceInformation() {
  return liquid::rhi::PhysicalDeviceInformation(
      "Mock Device", liquid::rhi::PhysicalDeviceType::CPU, {}, {});
}

const liquid::rhi::DeviceStats &MockRenderDevice::getDeviceStats() const {
  return mNoopDeviceStats;
}

void MockRenderDevice::destroyResources() {}

liquid::rhi::Swapchain MockRenderDevice::getSwapchain() {
  return liquid::rhi::Swapchain();
}

liquid::rhi::ShaderHandle MockRenderDevice::createShader(
    const liquid::rhi::ShaderDescription &description) {
  auto handle = getNewHandle<liquid::rhi::ShaderHandle>();
  mShaders.insert_or_assign(handle, description);
  return handle;
}

liquid::rhi::Buffer MockRenderDevice::createBuffer(
    const liquid::rhi::BufferDescription &description) {
  auto handle = getNewHandle<liquid::rhi::BufferHandle>();
  mBuffers.insert_or_assign(handle, MockBuffer{description});
  return liquid::rhi::Buffer(handle, &mBuffers.at(handle));
}

liquid::rhi::TextureHandle MockRenderDevice::createTexture(
    const liquid::rhi::TextureDescription &description) {
  auto handle = getNewHandle<liquid::rhi::TextureHandle>();
  mTextures.insert_or_assign(handle, description);
  return handle;
}

const liquid::rhi::TextureDescription MockRenderDevice::getTextureDescription(
    liquid::rhi::TextureHandle handle) const {
  return mTextures.at(handle);
}

liquid::rhi::RenderPassHandle MockRenderDevice::createRenderPass(
    const liquid::rhi::RenderPassDescription &description) {
  auto handle = getNewHandle<liquid::rhi::RenderPassHandle>();
  mRenderPasses.insert_or_assign(handle, description);
  return handle;
}

void MockRenderDevice::destroyRenderPass(
    const liquid::rhi::RenderPassHandle handle) {}

liquid::rhi::FramebufferHandle MockRenderDevice::createFramebuffer(
    const liquid::rhi::FramebufferDescription &description) {
  auto handle = getNewHandle<liquid::rhi::FramebufferHandle>();
  mFramebuffers.insert_or_assign(handle, description);
  return handle;
}

void MockRenderDevice::destroyFramebuffer(
    liquid::rhi::FramebufferHandle handle) {}