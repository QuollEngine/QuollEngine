#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Profiler.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDeviceObject.h"
#include "VulkanError.h"
#include "VulkanFramebuffer.h"
#include "VulkanHeaders.h"
#include "VulkanPipeline.h"
#include "VulkanRenderDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanResourceMetrics.h"
#include "VulkanSampler.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"

namespace quoll::rhi {

VulkanRenderDevice::VulkanRenderDevice(
    VulkanRenderBackend &backend, const VulkanPhysicalDevice &physicalDevice)
    : mPhysicalDevice(physicalDevice), mBackend(backend),
      mCommandPool(mDevice,
                   mPhysicalDevice.getQueueFamilyIndices().getGraphicsFamily(),
                   mRegistry, mDescriptorPool, mTimestampManager, mStats),
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
      mStats(new VulkanResourceMetrics(mRegistry, mDescriptorPool)),
      mTimestampManager(mDevice) {}

RenderCommandList VulkanRenderDevice::requestImmediateCommandList() {
  auto commandList = std::move(mCommandPool.createCommandLists(1).at(0));

  auto *commandBuffer = dynamic_cast<rhi::VulkanCommandBuffer *>(
                            commandList.getNativeRenderCommandList().get())
                            ->getVulkanCommandBuffer();

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  beginInfo.pInheritanceInfo = nullptr;

  checkForVulkanError(vkBeginCommandBuffer(commandBuffer, &beginInfo),
                      "Failed to begin recording command buffer");

  return std::move(commandList);
}

void VulkanRenderDevice::submitImmediate(RenderCommandList &commandList) {
  auto *commandBuffer = dynamic_cast<rhi::VulkanCommandBuffer *>(
                            commandList.getNativeRenderCommandList().get())
                            ->getVulkanCommandBuffer();

  vkEndCommandBuffer(commandBuffer);

  VkCommandBufferSubmitInfo commandBufferInfo{};
  commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  commandBufferInfo.pNext = nullptr;
  commandBufferInfo.commandBuffer = commandBuffer;
  commandBufferInfo.deviceMask = 0;

  std::array<VkCommandBufferSubmitInfo, 1> commandBufferInfos{
      commandBufferInfo};

  mGraphicsQueue.submit(VK_NULL_HANDLE, commandBufferInfos, {}, {});
  mGraphicsQueue.waitForIdle();
}

RenderFrame VulkanRenderDevice::beginFrame() {
  static constexpr auto SkipFrame = std::numeric_limits<u32>::max();
  static RenderCommandList emptyCommandList;

  QUOLL_PROFILE_EVENT("VulkanRenderDevice::beginFrame");

  mStats.resetCalls();
  mFrameManager.waitForFrame();

  const u32 imageIndex =
      mSwapchain.acquireNextImage(mFrameManager.getImageAvailableSemaphore());

  if (imageIndex == std::numeric_limits<u32>::max()) {
    recreateSwapchain();
    return {SkipFrame, SkipFrame, emptyCommandList};
  }

  auto &commandBuffer = mRenderContext.beginRendering(mFrameManager);
  mTimestampManager.setCurrentFrame(mFrameManager.getCurrentFrameIndex());
  mTimestampManager.reset();

  return {mFrameManager.getCurrentFrameIndex(), imageIndex, commandBuffer};
}

void VulkanRenderDevice::endFrame(const RenderFrame &renderFrame) {
  QUOLL_PROFILE_EVENT("VulkanRenderDevice::endFrame");
  mRenderContext.endRendering(mFrameManager);

  VkSwapchainKHR swapchainHandle = mSwapchain.getVulkanHandle();

  auto queuePresentResult = mRenderContext.present(
      mFrameManager, mSwapchain, renderFrame.swapchainImageIndex);

  if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
      queuePresentResult == VK_SUBOPTIMAL_KHR) {
    recreateSwapchain();
  }
  mFrameManager.nextFrame();
}

void VulkanRenderDevice::collectTimestamps(std::vector<u64> &timestamps) {
  vkGetQueryPoolResults(mDevice, mTimestampManager.getQueryPool(), 0,
                        static_cast<u32>(timestamps.size()),
                        timestamps.size() * sizeof(u64), timestamps.data(),
                        sizeof(u64),
                        VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
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

void VulkanRenderDevice::recreateSwapchain() {
  waitForIdle();
  mSwapchain.recreate(mBackend, mPhysicalDevice, mAllocator);
}

Buffer VulkanRenderDevice::createBuffer(const BufferDescription &description) {
  auto handle = mRegistry.setBuffer(
      std::make_unique<VulkanBuffer>(description, mAllocator, mDevice));

  return Buffer{handle, mRegistry.getBuffers().at(handle).get()};
}

void VulkanRenderDevice::destroyBuffer(BufferHandle handle) {
  mRegistry.deleteBuffer(handle);
}

void VulkanRenderDevice::createShader(const ShaderDescription &description,
                                      ShaderHandle handle) {
  mRegistry.setShader(std::make_unique<VulkanShader>(description, mDevice),
                      handle);
}

DescriptorLayoutHandle VulkanRenderDevice::createDescriptorLayout(
    const DescriptorLayoutDescription &description) {
  return mPipelineLayoutCache.getOrCreateDescriptorLayout(description);
}

Descriptor VulkanRenderDevice::createDescriptor(DescriptorLayoutHandle layout) {
  return mDescriptorPool.createDescriptor(layout);
}

void VulkanRenderDevice::createTexture(const TextureDescription &description,
                                       TextureHandle handle) {
  mRegistry.setTexture(
      std::make_unique<VulkanTexture>(description, mAllocator, mDevice),
      handle);
}

void VulkanRenderDevice::createSampler(const SamplerDescription &description,
                                       SamplerHandle handle) {
  mRegistry.setSampler(std::make_unique<VulkanSampler>(description, mDevice),
                       handle);
}

void VulkanRenderDevice::destroySampler(SamplerHandle handle) {
  mRegistry.deleteSampler(handle);
}

const TextureDescription
VulkanRenderDevice::getTextureDescription(TextureHandle handle) const {
  return mRegistry.getTextures().at(handle)->getDescription();
}

void VulkanRenderDevice::destroyTexture(TextureHandle handle) {
  mRegistry.deleteTexture(handle);
}

void VulkanRenderDevice::createTextureView(
    const TextureViewDescription &description, TextureHandle handle) {
  mRegistry.setTexture(std::make_unique<VulkanTexture>(description, mRegistry,
                                                       mAllocator, mDevice),
                       handle);
}

void VulkanRenderDevice::createRenderPass(
    const RenderPassDescription &description, RenderPassHandle handle) {
  mRegistry.setRenderPass(
      std::make_unique<VulkanRenderPass>(description, mDevice, mRegistry),
      handle);
}

void VulkanRenderDevice::destroyRenderPass(RenderPassHandle handle) {
  mRegistry.deleteRenderPass(handle);
}

void VulkanRenderDevice::createFramebuffer(
    const FramebufferDescription &description, FramebufferHandle handle) {
  mRegistry.setFramebuffer(
      std::make_unique<VulkanFramebuffer>(description, mDevice, mRegistry),
      handle);
}

void VulkanRenderDevice::destroyFramebuffer(FramebufferHandle handle) {
  mRegistry.deleteFramebuffer(handle);
}

void VulkanRenderDevice::createPipeline(
    const GraphicsPipelineDescription &description, PipelineHandle handle) {
  return mRegistry.setPipeline(
      std::make_unique<VulkanPipeline>(description, mDevice, mRegistry,
                                       mPipelineLayoutCache),
      handle);
}

void VulkanRenderDevice::createPipeline(
    const ComputePipelineDescription &description, PipelineHandle handle) {
  return mRegistry.setPipeline(
      std::make_unique<VulkanPipeline>(description, mDevice, mRegistry,
                                       mPipelineLayoutCache),
      handle);
}

void VulkanRenderDevice::destroyPipeline(PipelineHandle handle) {
  mRegistry.deletePipeline(handle);
}

bool VulkanRenderDevice::hasPipeline(PipelineHandle handle) {
  return mRegistry.hasPipeline(handle);
}

} // namespace quoll::rhi
