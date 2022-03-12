#include "liquid/core/Base.h"

#include <vulkan/vulkan.hpp>
#include "VulkanRenderDevice.h"
#include "VulkanDeviceObject.h"
#include "liquid/renderer/vulkan/VulkanError.h"

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
      mManager(mBackend, mPhysicalDevice, mDevice, mUploadContext) {}

VulkanRenderDevice::~VulkanRenderDevice() {
  for (const auto &[_, x] : mRegistry.getBuffers()) {
    mManager.destroyBuffer(x);
  }

  for (const auto &[_, x] : mRegistry.getTextures()) {
    mManager.destroyTexture(x);
  }
}

void VulkanRenderDevice::synchronize(ResourceRegistry &registry) {
  for (auto &handle : registry.getBufferMap().getDirtyCreates()) {
    auto &&buffer =
        mManager.createBuffer(registry.getBufferMap().getDescription(handle));
    mRegistry.addBuffer(handle, std::move(buffer));
  }

  registry.getBufferMap().clearDirtyCreates();

  for (auto &handle : registry.getBufferMap().getDirtyUpdates()) {
    if (registry.getBufferMap().hasDescription(handle)) {
      auto &buffer = mRegistry.getBuffer(handle);
      auto newBuffer = mManager.updateBuffer(
          buffer, registry.getBufferMap().getDescription(handle));
      if (newBuffer) {
        mRegistry.updateBuffer(handle, std::move(newBuffer));
      }
    }
  }
  registry.getBufferMap().clearDirtyUpdates();

  for (auto &handle : registry.getTextureMap().getDirtyCreates()) {
    if (registry.getTextureMap().hasDescription(handle)) {
      auto &&texture = mManager.createTexture(
          registry.getTextureMap().getDescription(handle));
      mRegistry.addTexture(handle, std::move(texture));
    }
  }
  registry.getTextureMap().clearDirtyCreates();
}

void VulkanRenderDevice::synchronizeDeletes(ResourceRegistry &registry) {
  for (auto &handle : registry.getBufferMap().getDirtyDeletes()) {
    mManager.destroyBuffer(mRegistry.getBuffer(handle));
    mRegistry.removeBuffer(handle);
  }

  registry.getBufferMap().clearDirtyDeletes();

  for (auto &handle : registry.getTextureMap().getDirtyDeletes()) {
    mManager.destroyTexture(mRegistry.getTexture(handle));
    mRegistry.removeTexture(handle);
  }

  registry.getBufferMap().clearDirtyDeletes();
}

void VulkanRenderDevice::wait() { vkDeviceWaitIdle(mDevice); }

} // namespace liquid::experimental
