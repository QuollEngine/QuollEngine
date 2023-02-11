#include "liquid/core/Base.h"
#include "MockRenderDevice.h"
#include "MockRenderCommandList.h"

liquid::rhi::RenderCommandList MockRenderDevice::requestImmediateCommandList() {
  return liquid::rhi::RenderCommandList(new MockRenderCommandList);
}

void MockRenderDevice::submitImmediate(
    liquid::rhi::RenderCommandList &commandList) {}

liquid::rhi::RenderFrame MockRenderDevice::beginFrame() {
  return liquid::rhi::RenderFrame{std::numeric_limits<uint32_t>::max(),
                                  std::numeric_limits<uint32_t>::max(),
                                  mNoopCommandList};
}

void MockRenderDevice::endFrame(const liquid::rhi::RenderFrame &renderFrame) {}

void MockRenderDevice::waitForIdle() {}

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

liquid::rhi::DescriptorLayoutHandle MockRenderDevice::createDescriptorLayout(
    const liquid::rhi::DescriptorLayoutDescription &description) {
  auto handle = getNewHandle<liquid::rhi::DescriptorLayoutHandle>();
  mDescriptorLayouts.insert_or_assign(handle, description);
  return handle;
}

liquid::rhi::Descriptor
MockRenderDevice::createDescriptor(liquid::rhi::DescriptorLayoutHandle layout) {
  auto handle = getNewHandle<liquid::rhi::DescriptorHandle>();
  mDescriptors.insert_or_assign(handle, MockDescriptor{layout});

  return liquid::rhi::Descriptor(&mDescriptors.at(handle), handle);
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

void MockRenderDevice::destroyBuffer(liquid::rhi::BufferHandle handle) {}

liquid::rhi::TextureHandle MockRenderDevice::createTexture(
    const liquid::rhi::TextureDescription &description) {
  auto handle = getNewHandle<liquid::rhi::TextureHandle>();
  mTextures.insert_or_assign(handle, description);
  return handle;
}

void MockRenderDevice::destroyTexture(liquid::rhi::TextureHandle handle) {}

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

liquid::rhi::PipelineHandle MockRenderDevice::createPipeline(
    const liquid::rhi::GraphicsPipelineDescription &description) {
  auto handle = getNewHandle<liquid::rhi::PipelineHandle>();
  mPipelines.insert_or_assign(handle, description);
  return handle;
}

liquid::rhi::PipelineHandle MockRenderDevice::createPipeline(
    const liquid::rhi::ComputePipelineDescription &description) {
  auto handle = getNewHandle<liquid::rhi::PipelineHandle>();
  mComputePipelines.insert_or_assign(handle, description);
  return handle;
}

void MockRenderDevice::destroyPipeline(liquid::rhi::PipelineHandle handle) {}

size_t MockRenderDevice::addTextureUpdateListener(
    const std::function<void(const std::set<liquid::rhi::TextureHandle> &)>
        &listener) {
  return 0;
}

void MockRenderDevice::removeTextureUpdateListener(size_t handle) {}
