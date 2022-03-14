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

#include "VulkanError.h"
#include "liquid/core/EngineGlobals.h"

namespace liquid::experimental {

VulkanRenderDevice::VulkanRenderDevice(
    VulkanRenderBackend &backend, const VulkanPhysicalDevice &physicalDevice)
    : mPhysicalDevice(physicalDevice), mBackend(backend),
      mCommandPool(
          mDevice,
          mPhysicalDevice.getQueueFamilyIndices().graphicsFamily.value(),
          mRegistry, mDescriptorManager),
      mDevice(mPhysicalDevice), mDescriptorManager(mDevice, mRegistry),
      mGraphicsQueue(
          mDevice,
          mPhysicalDevice.getQueueFamilyIndices().graphicsFamily.value()),
      mPresentQueue(
          mDevice,
          mPhysicalDevice.getQueueFamilyIndices().presentFamily.value()),
      mRenderContext(mDevice, mCommandPool, mGraphicsQueue, mPresentQueue),
      mUploadContext(mDevice, mCommandPool, mGraphicsQueue),
      mSwapchain(mBackend, mPhysicalDevice, mDevice, VK_NULL_HANDLE),
      mAllocator(mBackend, mPhysicalDevice, mDevice) {

  synchronizeSwapchain(0);
}

void VulkanRenderDevice::synchronizeSwapchain(size_t prevNumSwapchainImages) {

  auto numNewSwapchainImages = mSwapchain.getImages().size();

  // Remove unused swapchain images if
  // new swapchain has fewer images than
  // previous one
  for (size_t i = numNewSwapchainImages + 1; i < prevNumSwapchainImages; ++i) {
    mRegistry.removeTexture(static_cast<TextureHandle>(i));
  }

  auto numSwapchainImages = static_cast<TextureHandle>(numNewSwapchainImages);

  for (size_t i = 0; i < numSwapchainImages; ++i) {
    TextureHandle handle = static_cast<TextureHandle>(i + 1);

    auto &&ptr = std::make_unique<VulkanTexture>(
        mSwapchain.getImages().at(i), mSwapchain.getImageViews().at(i),
        VK_NULL_HANDLE, mSwapchain.getSurfaceFormat().format, mAllocator,
        mDevice);

    if (mRegistry.getTextures().find(handle) != mRegistry.getTextures().end()) {
      mRegistry.updateTexture(handle, std::move(ptr));
    } else {
      mRegistry.addTexture(handle, std::move(ptr));
    }
  }
}

void VulkanRenderDevice::synchronize(ResourceRegistry &registry) {
  // Shaders
  for (auto &handle : registry.getShaderMap().getDirtyCreates()) {
    mRegistry.addShader(
        handle, std::make_unique<VulkanShader>(
                    registry.getShaderMap().getDescription(handle), mDevice));
  }

  registry.getShaderMap().clearDirtyCreates();

  // Buffers
  for (auto &handle : registry.getBufferMap().getDirtyCreates()) {
    mRegistry.addBuffer(
        handle,
        std::make_unique<VulkanBuffer>(
            registry.getBufferMap().getDescription(handle), mAllocator));
  }
  registry.getBufferMap().clearDirtyCreates();

  for (auto &handle : registry.getBufferMap().getDirtyUpdates()) {
    if (registry.getBufferMap().hasDescription(handle)) {
      mRegistry.getBuffer(handle)->update(
          registry.getBufferMap().getDescription(handle));
    }
  }
  registry.getBufferMap().clearDirtyUpdates();

  // Textures
  for (auto &handle : registry.getTextureMap().getDirtyCreates()) {
    if (registry.getTextureMap().hasDescription(handle)) {
      mRegistry.addTexture(handle,
                           std::make_unique<VulkanTexture>(
                               registry.getTextureMap().getDescription(handle),
                               mAllocator, mDevice, mUploadContext));
    }
  }
  registry.getTextureMap().clearDirtyCreates();

  // Render passes
  for (auto &handle : registry.getRenderPassMap().getDirtyCreates()) {
    mRegistry.addRenderPass(
        handle, std::make_unique<VulkanRenderPass>(
                    registry.getRenderPassMap().getDescription(handle), mDevice,
                    mRegistry));
  }
  registry.getRenderPassMap().clearDirtyCreates();

  for (auto &handle : registry.getRenderPassMap().getDirtyUpdates()) {
    if (registry.getRenderPassMap().hasDescription(handle)) {
      mRegistry.updateRenderPass(
          handle, std::make_unique<VulkanRenderPass>(
                      registry.getRenderPassMap().getDescription(handle),
                      mDevice, mRegistry));
    }
  }
  registry.getRenderPassMap().clearDirtyUpdates();

  // Framebuffers
  for (auto &handle : registry.getFramebufferMap().getDirtyCreates()) {
    mRegistry.addFramebuffer(
        handle, std::make_unique<VulkanFramebuffer>(
                    registry.getFramebufferMap().getDescription(handle),
                    mDevice, mRegistry));
  }
  registry.getFramebufferMap().clearDirtyCreates();

  for (auto &handle : registry.getFramebufferMap().getDirtyUpdates()) {
    if (registry.getFramebufferMap().hasDescription(handle)) {
      mRegistry.updateFramebuffer(
          handle, std::make_unique<VulkanFramebuffer>(
                      registry.getFramebufferMap().getDescription(handle),
                      mDevice, mRegistry));
    }
  }
  registry.getFramebufferMap().clearDirtyUpdates();

  // Pipelines
  for (auto &handle : registry.getPipelineMap().getDirtyCreates()) {
    mRegistry.addPipeline(handle,
                          std::make_unique<VulkanPipeline>(
                              registry.getPipelineMap().getDescription(handle),
                              mDevice, mRegistry));
  }

  registry.getPipelineMap().clearDirtyCreates();

  for (auto &handle : registry.getPipelineMap().getDirtyUpdates()) {
    if (registry.getPipelineMap().hasDescription(handle)) {
      mRegistry.updatePipeline(
          handle, std::make_unique<VulkanPipeline>(
                      registry.getPipelineMap().getDescription(handle), mDevice,
                      mRegistry));
    }
  }
  registry.getPipelineMap().clearDirtyUpdates();
}

void VulkanRenderDevice::synchronizeDeletes(ResourceRegistry &registry) {
  for (auto &handle : registry.getBufferMap().getDirtyDeletes()) {
    mRegistry.removeBuffer(handle);
  }

  registry.getBufferMap().clearDirtyDeletes();

  for (auto &handle : registry.getTextureMap().getDirtyDeletes()) {
    mRegistry.removeTexture(handle);
  }

  registry.getBufferMap().clearDirtyDeletes();

  // Shaders
  for (auto &handle : registry.getShaderMap().getDirtyDeletes()) {
    mRegistry.removeShader(handle);
  }

  registry.getShaderMap().clearDirtyDeletes();
}

void VulkanRenderDevice::execute(RenderGraph &graph,
                                 RenderGraphEvaluator &evaluator) {
  LIQUID_PROFILE_EVENT("VulkanRenderDevice::execute");

  uint32_t imageIdx =
      mSwapchain.acquireNextImage(mRenderContext.getImageAvailableSemaphore());

  if (imageIdx == std::numeric_limits<uint32_t>::max()) {
    recreateSwapchain();
    return;
  }

  auto &&compiled =
      evaluator.compile(graph, mSwapchainRecreated, mSwapchain.getExtent());

  evaluator.build(compiled, graph, mSwapchainRecreated,
                  static_cast<uint32_t>(mSwapchain.getImages().size()),
                  mSwapchain.getExtent());

  synchronize(evaluator.getRegistry());

  if (mSwapchainRecreated) {
    mSwapchainRecreated = false;
  }

  auto &commandBuffer = mRenderContext.beginRendering();
  evaluator.execute(commandBuffer, compiled, graph, imageIdx);
  mRenderContext.endRendering();

  auto queuePresentResult = mRenderContext.present(mSwapchain, imageIdx);
  synchronizeDeletes(evaluator.getRegistry());

  if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
      queuePresentResult == VK_SUBOPTIMAL_KHR ||
      mBackend.isFramebufferResized()) {
    recreateSwapchain();
  }
}

void VulkanRenderDevice::wait() { vkDeviceWaitIdle(mDevice); }

void VulkanRenderDevice::recreateSwapchain() {
  wait();
  size_t prevNumSwapchainImages = mSwapchain.getImageViews().size();

  mSwapchain = VulkanSwapchain(mBackend, mPhysicalDevice, mDevice,
                               mSwapchain.getSwapchain());

  synchronizeSwapchain(prevNumSwapchainImages);
  mBackend.finishFramebufferResize();
  mSwapchainRecreated = true;
}

} // namespace liquid::experimental
