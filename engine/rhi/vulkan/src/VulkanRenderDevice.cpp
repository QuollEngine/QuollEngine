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

#include "VulkanError.h"
#include "liquid/core/EngineGlobals.h"

namespace liquid::rhi {

VulkanRenderDevice::VulkanRenderDevice(
    VulkanRenderBackend &backend, const VulkanPhysicalDevice &physicalDevice)
    : mPhysicalDevice(physicalDevice), mBackend(backend),
      mCommandPool(mDevice,
                   mPhysicalDevice.getQueueFamilyIndices().getGraphicsFamily(),
                   mRegistry, mDescriptorManager, mStats),
      mDevice(mPhysicalDevice), mDescriptorManager(mDevice, mRegistry),
      mGraphicsQueue(
          mDevice, mPhysicalDevice.getQueueFamilyIndices().getGraphicsFamily()),
      mPresentQueue(mDevice,
                    mPhysicalDevice.getQueueFamilyIndices().getPresentFamily()),
      mFrameManager(mDevice),
      mRenderContext(mDevice, mCommandPool, mGraphicsQueue, mPresentQueue),
      mUploadContext(mDevice, mCommandPool, mGraphicsQueue),
      mSwapchain(mBackend, mPhysicalDevice, mDevice, mRegistry, mAllocator),
      mAllocator(mBackend, mPhysicalDevice, mDevice) {

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
  mSwapchain.recreate(mBackend, mPhysicalDevice, mAllocator);
}

Swapchain VulkanRenderDevice::getSwapchain() {
  return Swapchain{mSwapchain.getTextures(), mSwapchain.getExtent()};
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
    mRegistry.setTexture(handle,
                         std::make_unique<VulkanTexture>(
                             texture->getDescription(), mAllocator, mDevice,
                             mUploadContext, mSwapchain.getExtent()));
  }
}

void VulkanRenderDevice::synchronize(ResourceRegistry &registry) {
  LIQUID_PROFILE_EVENT("VulkanRenderDevice::synchronize");
  // Shaders
  for (auto [handle, state] : registry.getShaderMap().getStagedResources()) {
    if (state == ResourceRegistryState::Set) {
      mRegistry.setShader(
          handle, std::make_unique<VulkanShader>(
                      registry.getShaderMap().getDescription(handle), mDevice));
    } else {
      mRegistry.deleteShader(handle);
    }
  }

  registry.getShaderMap().clearStagedResources();

  // Buffers
  for (auto [handle, state] : registry.getBufferMap().getStagedResources()) {
    if (state == ResourceRegistryState::Set) {
      if (mRegistry.hasBuffer(handle)) {
        mRegistry.getBuffers().at(handle)->update(
            registry.getBufferMap().getDescription(handle));
      } else {
        mRegistry.setBuffer(
            handle,
            std::make_unique<VulkanBuffer>(
                registry.getBufferMap().getDescription(handle), mAllocator));
      }
    } else {
      mRegistry.deleteBuffer(handle);
    }
  }

  registry.getBufferMap().clearStagedResources();

  // Textures
  for (auto [handle, state] : registry.getTextureMap().getStagedResources()) {
    if (state == ResourceRegistryState::Set) {
      mRegistry.setTexture(handle,
                           std::make_unique<VulkanTexture>(
                               registry.getTextureMap().getDescription(handle),
                               mAllocator, mDevice, mUploadContext,
                               mSwapchain.getExtent()));
    } else {
      mRegistry.deleteTexture(handle);
    }
  }

  registry.getTextureMap().clearStagedResources();

  // Render passes
  for (auto [handle, state] :
       registry.getRenderPassMap().getStagedResources()) {
    if (state == ResourceRegistryState::Set) {
      mRegistry.setRenderPass(
          handle, std::make_unique<VulkanRenderPass>(
                      registry.getRenderPassMap().getDescription(handle),
                      mDevice, mRegistry));
    } else {
      mRegistry.deleteRenderPass(handle);
    }
  }

  registry.getRenderPassMap().clearStagedResources();

  // Framebuffers
  for (auto [handle, state] :
       registry.getFramebufferMap().getStagedResources()) {
    if (state == ResourceRegistryState::Set) {
      mRegistry.setFramebuffer(
          handle, std::make_unique<VulkanFramebuffer>(
                      registry.getFramebufferMap().getDescription(handle),
                      mDevice, mRegistry));
    } else {
      mRegistry.deleteFramebuffer(handle);
    }
  }

  registry.getFramebufferMap().clearStagedResources();

  // Pipelines
  for (auto [handle, state] : registry.getPipelineMap().getStagedResources()) {
    if (state == ResourceRegistryState::Set) {

      mRegistry.setPipeline(
          handle, std::make_unique<VulkanPipeline>(
                      registry.getPipelineMap().getDescription(handle), mDevice,
                      mRegistry));
    } else {
      mRegistry.deletePipeline(handle);
    }
  }

  registry.getPipelineMap().clearStagedResources();
}

} // namespace liquid::rhi
