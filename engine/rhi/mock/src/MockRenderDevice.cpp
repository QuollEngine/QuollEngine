#include "liquid/core/Base.h"
#include "MockRenderDevice.h"
#include "MockResourceMetrics.h"

namespace liquid::rhi {

MockRenderDevice::MockRenderDevice() : mDeviceStats(new MockResourceMetrics) {
  for (auto i = 0; i < NumFrames; ++i) {
    mCommandLists.at(i) = RenderCommandList(new MockCommandList);
  }
}

RenderCommandList MockRenderDevice::requestImmediateCommandList() {
  return RenderCommandList(new MockCommandList);
}

void MockRenderDevice::submitImmediate(RenderCommandList &commandList) {
  auto *mockCommandList = static_cast<MockCommandList *>(
      commandList.getNativeRenderCommandList().get());
  mSubmittedCommandLists.push_back(std::move(*mockCommandList));
  mockCommandList->clear();
}

RenderFrame MockRenderDevice::beginFrame() {
  auto frameIndex = mFrameIndex;
  mFrameIndex = (mFrameIndex + 1) % NumFrames;

  return RenderFrame{frameIndex, 0, mCommandLists.at(mFrameIndex)};
}

void MockRenderDevice::endFrame(const RenderFrame &renderFrame) {
  auto *mockCommandList = static_cast<MockCommandList *>(
      renderFrame.commandList.getNativeRenderCommandList().get());
  mSubmittedCommandLists.push_back(std::move(*mockCommandList));
  mockCommandList->clear();
}

void MockRenderDevice::waitForIdle() {
  // Do nothing
}

const PhysicalDeviceInformation MockRenderDevice::getDeviceInformation() {
  return PhysicalDeviceInformation("MockDevice", {}, {});
}

const DeviceStats &MockRenderDevice::getDeviceStats() const {
  return mDeviceStats;
}

void MockRenderDevice::destroyResources() {
  mBuffers.clear();
  mTextures.clear();
  mTextureViews.clear();
  mFramebuffers.clear();
  mRenderPasses.clear();
  mShaders.clear();
  mDescriptorLayouts.clear();
  mDescriptors.clear();
}

Swapchain MockRenderDevice::getSwapchain() { return Swapchain(); }

void MockRenderDevice::recreateSwapchain() {
  // Do nothing
}

ShaderHandle
MockRenderDevice::createShader(const ShaderDescription &description) {
  return mShaders.insert(description);
}

DescriptorLayoutHandle MockRenderDevice::createDescriptorLayout(
    const DescriptorLayoutDescription &description) {
  return mDescriptorLayouts.insert(description);
}

Descriptor MockRenderDevice::createDescriptor(DescriptorLayoutHandle layout) {
  auto *mockDescriptor = new MockDescriptor(layout);
  auto handle = mDescriptors.insert(
      std::move(std::unique_ptr<MockDescriptor>(mockDescriptor)));

  return Descriptor(mockDescriptor, handle);
}

Buffer MockRenderDevice::createBuffer(const BufferDescription &description) {
  auto *mockBuffer = new MockBuffer(description);

  auto handle =
      mBuffers.insert(std::move(std::unique_ptr<MockBuffer>(mockBuffer)));
  return Buffer(handle, mockBuffer);
}

void MockRenderDevice::destroyBuffer(BufferHandle handle) {
  mBuffers.erase(handle);
}

TextureHandle
MockRenderDevice::createTexture(const TextureDescription &description) {
  return mTextures.insert({description});
}

void MockRenderDevice::updateTexture(TextureHandle handle,
                                     const TextureDescription &description) {
  mTextures.replace(handle, {description});
}

const TextureDescription
MockRenderDevice::getTextureDescription(TextureHandle handle) const {
  return mTextures.at(handle).getDescription();
}

void MockRenderDevice::destroyTexture(TextureHandle handle) {
  mTextures.erase(handle);
}

TextureViewHandle
MockRenderDevice::createTextureView(const TextureViewDescription &description) {
  return mTextureViews.insert(description);
}

void MockRenderDevice::destroyTextureView(TextureViewHandle handle) {
  mTextureViews.erase(handle);
}

RenderPassHandle
MockRenderDevice::createRenderPass(const RenderPassDescription &description) {
  return mRenderPasses.insert(description);
}

void MockRenderDevice::destroyRenderPass(RenderPassHandle handle) {
  mRenderPasses.erase(handle);
}

FramebufferHandle
MockRenderDevice::createFramebuffer(const FramebufferDescription &description) {
  return mFramebuffers.insert(description);
}

void MockRenderDevice::destroyFramebuffer(FramebufferHandle handle) {
  mFramebuffers.erase(handle);
}

void MockRenderDevice::createPipeline(
    const GraphicsPipelineDescription &description, PipelineHandle handle) {
  mPipelines.insert({description}, handle);
}

void MockRenderDevice::createPipeline(
    const ComputePipelineDescription &description, PipelineHandle handle) {
  mPipelines.insert({description}, handle);
}

void MockRenderDevice::destroyPipeline(PipelineHandle handle) {
  mPipelines.erase(handle);
}

} // namespace liquid::rhi
