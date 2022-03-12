#include "liquid/core/Base.h"

#include <vulkan/vulkan.hpp>
#include "VulkanRenderDevice.h"
#include "VulkanDeviceObject.h"
#include "liquid/renderer/vulkan/VulkanError.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"

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
      mAllocator(mBackend, mPhysicalDevice, mDevice) {}

void VulkanRenderDevice::synchronize(ResourceRegistry &registry) {
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

  for (auto &handle : registry.getTextureMap().getDirtyCreates()) {
    if (registry.getTextureMap().hasDescription(handle)) {
      mRegistry.addTexture(handle,
                           std::make_unique<VulkanTexture>(
                               registry.getTextureMap().getDescription(handle),
                               mAllocator, mDevice, mUploadContext));
    }
  }
  registry.getTextureMap().clearDirtyCreates();
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
}

void VulkanRenderDevice::wait() { vkDeviceWaitIdle(mDevice); }

} // namespace liquid::experimental
