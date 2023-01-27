#include "liquid/core/Base.h"

#include <vulkan/vulkan.hpp>
#include "VulkanRenderDevice.h"
#include "VulkanDeviceObject.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanCommandBuffer.h"
#include "VulkanResourceMetrics.h"

#include "VulkanError.h"
#include "liquid/core/Engine.h"

namespace liquid::rhi {

VulkanRenderDevice::VulkanRenderDevice(
    VulkanRenderBackend &backend, const VulkanPhysicalDevice &physicalDevice)
    : mPhysicalDevice(physicalDevice), mBackend(backend),
      mCommandPool(mDevice,
                   mPhysicalDevice.getQueueFamilyIndices().getGraphicsFamily(),
                   mRegistry, mDescriptorPool, mStats),
      mDevice(mPhysicalDevice), mPipelineLayoutCache(mDevice),
      mDescriptorPool(mDevice, mRegistry, mPipelineLayoutCache),
      mGraphicsQueue(
          mDevice, mPhysicalDevice.getQueueFamilyIndices().getGraphicsFamily()),
      mPresentQueue(mDevice,
                    mPhysicalDevice.getQueueFamilyIndices().getPresentFamily()),
      mFrameManager(mDevice),
      mRenderContext(mDevice, mCommandPool, mGraphicsQueue, mPresentQueue),
      mUploadContext(mDevice, mCommandPool, mGraphicsQueue),
      mSwapchain(mBackend, mPhysicalDevice, mDevice, mRegistry, mAllocator),
      mAllocator(mBackend, mPhysicalDevice, mDevice),
      mStats(new VulkanResourceMetrics(mRegistry, mDescriptorPool)) {

  VkDevice device = mDevice.getVulkanHandle();
  VkPhysicalDevice physicalDeviceHandle = mPhysicalDevice.getVulkanHandle();
  VkQueue graphicsQueue = mGraphicsQueue.getVulkanHandle();
  uint32_t queueIndex = mGraphicsQueue.getQueueIndex();

  LIQUID_PROFILE_GPU_INIT_VULKAN(&device, &physicalDeviceHandle, &graphicsQueue,
                                 &queueIndex, 1, nullptr);
}

RenderFrame VulkanRenderDevice::beginFrame() {
  static constexpr auto SkipFrame = std::numeric_limits<uint32_t>::max();
  static RenderCommandList emptyCommandList;

  LIQUID_PROFILE_EVENT("VulkanRenderDevice::beginFrame");

  if (mBackend.isFramebufferResized()) {
    recreateSwapchain();
    return {SkipFrame, SkipFrame, emptyCommandList};
  }

  mStats.resetCalls();
  mFrameManager.waitForFrame();

  uint32_t imageIndex =
      mSwapchain.acquireNextImage(mFrameManager.getImageAvailableSemaphore());

  if (imageIndex == std::numeric_limits<uint32_t>::max()) {
    recreateSwapchain();
    return {SkipFrame, SkipFrame, emptyCommandList};
  }

  auto &commandBuffer = mRenderContext.beginRendering(mFrameManager);

  return {mFrameManager.getCurrentFrameIndex(), imageIndex, commandBuffer};
}

void VulkanRenderDevice::endFrame(const RenderFrame &renderFrame) {
  LIQUID_PROFILE_EVENT("VulkanRenderDevice::endFrame");
  mSwapchainRecreated = false;

  mRenderContext.endRendering(mFrameManager);

  VkSwapchainKHR swapchainHandle = mSwapchain.getVulkanHandle();
  LIQUID_PROFILE_GPU_FLIP(&mSwapchain);

  auto queuePresentResult = mRenderContext.present(
      mFrameManager, mSwapchain, renderFrame.swapchainImageIndex);

  if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
      queuePresentResult == VK_SUBOPTIMAL_KHR) {
    recreateSwapchain();
  }
  mFrameManager.nextFrame();
}

void VulkanRenderDevice::waitForIdle() { vkDeviceWaitIdle(mDevice); }

void VulkanRenderDevice::destroyResources() {
  waitForIdle();
  mRegistry = VulkanResourceRegistry();
  mPipelineLayoutCache.clear();
  mDescriptorPool.reset();

  mSwapchain.recreate(mBackend, mPhysicalDevice, mAllocator);
}

Swapchain VulkanRenderDevice::getSwapchain() {
  return Swapchain{mSwapchain.getTextures(), mSwapchain.getExtent()};
}

Buffer VulkanRenderDevice::createBuffer(const BufferDescription &description) {
  auto handle = mRegistry.setBuffer(
      std::make_unique<VulkanBuffer>(description, mAllocator));

  return Buffer{handle, mRegistry.getBuffers().at(handle).get()};
}

void VulkanRenderDevice::destroyBuffer(BufferHandle handle) {
  mRegistry.deleteBuffer(handle);
}

ShaderHandle
VulkanRenderDevice::createShader(const ShaderDescription &description) {
  return mRegistry.setShader(
      std::make_unique<VulkanShader>(description, mDevice));
}

DescriptorLayoutHandle VulkanRenderDevice::createDescriptorLayout(
    const DescriptorLayoutDescription &description) {
  return mPipelineLayoutCache.getOrCreateDescriptorLayout(description);
}

Descriptor VulkanRenderDevice::createDescriptor(DescriptorLayoutHandle layout) {
  return mDescriptorPool.createDescriptor(layout);
}

TextureHandle
VulkanRenderDevice::createTexture(const TextureDescription &description) {
  auto handle = mRegistry.setTexture(
      std::make_unique<VulkanTexture>(description, mAllocator, mDevice,
                                      mUploadContext, mSwapchain.getExtent()));

  return handle;
}

const TextureDescription
VulkanRenderDevice::getTextureDescription(TextureHandle handle) const {
  return mRegistry.getTextures().at(handle)->getDescription();
}

RenderPassHandle
VulkanRenderDevice::createRenderPass(const RenderPassDescription &description) {
  return mRegistry.setRenderPass(
      std::make_unique<VulkanRenderPass>(description, mDevice, mRegistry));
}

void VulkanRenderDevice::destroyRenderPass(RenderPassHandle handle) {
  mRegistry.deleteRenderPass(handle);
}

FramebufferHandle VulkanRenderDevice::createFramebuffer(
    const FramebufferDescription &description) {
  return mRegistry.setFramebuffer(
      std::make_unique<VulkanFramebuffer>(description, mDevice, mRegistry));
}

void VulkanRenderDevice::destroyFramebuffer(FramebufferHandle handle) {
  mRegistry.deleteFramebuffer(handle);
}

PipelineHandle
VulkanRenderDevice::createPipeline(const PipelineDescription &description) {
  return mRegistry.setPipeline(std::make_unique<VulkanPipeline>(
      description, mDevice, mRegistry, mPipelineLayoutCache));
}

PipelineHandle VulkanRenderDevice::createPipeline(
    const ComputePipelineDescription &description) {
  return mRegistry.setPipeline(std::make_unique<VulkanPipeline>(
      description, mDevice, mRegistry, mPipelineLayoutCache));
}

void VulkanRenderDevice::destroyPipeline(PipelineHandle handle) {
  mRegistry.deletePipeline(handle);
}

size_t VulkanRenderDevice::addTextureUpdateListener(
    const std::function<void(const std::set<TextureHandle> &)> &listener) {
  mTextureUpdateListeners.push_back(listener);

  return mTextureUpdateListeners.size() - 1;
}

void VulkanRenderDevice::removeTextureUpdateListener(size_t handle) {
  mTextureUpdateListeners.erase(mTextureUpdateListeners.begin() +
                                static_cast<int32_t>(handle));
}

void VulkanRenderDevice::recreateSwapchain() {
  waitForIdle();
  size_t prevNumSwapchainImages = mSwapchain.getTextures().size();

  mSwapchain.recreate(mBackend, mPhysicalDevice, mAllocator);

  updateFramebufferRelativeTextures();
  mBackend.finishFramebufferResize();
  mSwapchainRecreated = true;
}

void VulkanRenderDevice::updateFramebufferRelativeTextures() {
  mRegistry.deleteDanglingSwapchainRelativeTextures();

  for (auto handle : mRegistry.getSwapchainRelativeTextures()) {
    auto &texture = mRegistry.getTextures().at(handle);
    mRegistry.recreateTexture(handle, std::make_unique<VulkanTexture>(
                                          texture->getDescription(), mAllocator,
                                          mDevice, mUploadContext,
                                          mSwapchain.getExtent()));
  }

  for (const auto &listener : mTextureUpdateListeners) {
    listener(mRegistry.getSwapchainRelativeTextures());
  }
}

} // namespace liquid::rhi
