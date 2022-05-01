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
      mRenderContext(mDevice, mCommandPool, mGraphicsQueue, mPresentQueue),
      mUploadContext(mDevice, mCommandPool, mGraphicsQueue),
      mSwapchain(mBackend, mPhysicalDevice, mDevice, VK_NULL_HANDLE),
      mAllocator(mBackend, mPhysicalDevice, mDevice) {

  VkDevice device = mDevice.getVulkanHandle();
  VkPhysicalDevice physicalDeviceHandle = mPhysicalDevice.getVulkanHandle();
  VkQueue graphicsQueue = mGraphicsQueue.getVulkanHandle();
  uint32_t queueIndex = mGraphicsQueue.getQueueIndex();

  LIQUID_PROFILE_GPU_INIT_VULKAN(&device, &physicalDeviceHandle, &graphicsQueue,
                                 &queueIndex, 1, nullptr);

  synchronizeSwapchain(0);
}

void VulkanRenderDevice::execute(RenderGraph &graph,
                                 RenderGraphEvaluator &evaluator) {
  LIQUID_PROFILE_EVENT("VulkanRenderDevice::execute");
  mStats.resetCalls();

  uint32_t imageIdx =
      mSwapchain.acquireNextImage(mRenderContext.getImageAvailableSemaphore());

  if (imageIdx == std::numeric_limits<uint32_t>::max()) {
    synchronize(evaluator.getRegistry());
    recreateSwapchain();
    return;
  }

  auto &&compiled = graph.compile();

  evaluator.build(compiled, graph, mSwapchainRecreated,
                  static_cast<uint32_t>(mSwapchain.getImages().size()),
                  mSwapchain.getExtent());

  synchronize(evaluator.getRegistry());

  if (mSwapchainRecreated) {
    mSwapchainRecreated = false;
  }

  auto &commandBuffer = mRenderContext.beginRendering();

  auto *commandBufferHandle =
      dynamic_cast<VulkanCommandBuffer *>(
          commandBuffer.getNativeRenderCommandList().get())
          ->getVulkanCommandBuffer();
  LIQUID_PROFILE_GPU_CONTEXT(commandBufferHandle);
  {
    LIQUID_PROFILE_GPU_EVENT("GPU event");
    evaluator.execute(commandBuffer, compiled, graph, imageIdx);
  }
  mRenderContext.endRendering();

  VkSwapchainKHR swapchainHandle = mSwapchain.getVulkanHandle();
  LIQUID_PROFILE_GPU_FLIP(&mSwapchain);

  auto queuePresentResult = mRenderContext.present(mSwapchain, imageIdx);

  if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
      queuePresentResult == VK_SUBOPTIMAL_KHR ||
      mBackend.isFramebufferResized()) {
    recreateSwapchain();
  }
}

void VulkanRenderDevice::waitForIdle() { vkDeviceWaitIdle(mDevice); }

void VulkanRenderDevice::destroyResources() {
  mRegistry = VulkanResourceRegistry();
  recreateSwapchain();
}

void VulkanRenderDevice::recreateSwapchain() {
  waitForIdle();
  size_t prevNumSwapchainImages = mSwapchain.getImageViews().size();

  mSwapchain = VulkanSwapchain(mBackend, mPhysicalDevice, mDevice, mSwapchain);

  synchronizeSwapchain(prevNumSwapchainImages);
  mBackend.finishFramebufferResize();
  mSwapchainRecreated = true;

  for (auto handle : mRegistry.getSwapchainRelativeTextures()) {
    auto &texture = mRegistry.getTextures().at(handle);
    mRegistry.setTexture(handle,
                         std::make_unique<VulkanTexture>(
                             texture->getDescription(), mAllocator, mDevice,
                             mUploadContext, mSwapchain.getExtent()));
  }
}

void VulkanRenderDevice::synchronizeSwapchain(size_t prevNumSwapchainImages) {
  auto numNewSwapchainImages = mSwapchain.getImages().size();

  // Remove unused swapchain images if
  // new swapchain has fewer images than
  // previous one
  for (size_t i = numNewSwapchainImages + 1; i < prevNumSwapchainImages; ++i) {
    mRegistry.deleteTexture(static_cast<TextureHandle>(i));
  }

  for (size_t i = 0; i < numNewSwapchainImages; ++i) {
    TextureHandle handle = static_cast<TextureHandle>(i + 1);

    mRegistry.setTexture(
        handle, std::make_unique<VulkanTexture>(
                    mSwapchain.getImages().at(i),
                    mSwapchain.getImageViews().at(i), VK_NULL_HANDLE,
                    mSwapchain.getSurfaceFormat().format, mAllocator, mDevice));
  }

  // Recreate swapchain relative textures
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
