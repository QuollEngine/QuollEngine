#pragma once

#include "../BufferDescription.h"
#include "../TextureDescription.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanRenderBackend.h"
#include "VulkanUploadContext.h"

namespace liquid::experimental {

class VulkanResourceManager {
public:
  VulkanResourceManager(VulkanRenderBackend &backend,
                        VulkanPhysicalDevice &physicalDevice,
                        VulkanDeviceObject &device,
                        VulkanUploadContext &uploadContext);

  ~VulkanResourceManager();

  VulkanResourceManager(const VulkanResourceManager &) = delete;
  VulkanResourceManager &operator=(const VulkanResourceManager &) = delete;
  VulkanResourceManager(VulkanResourceManager &&) = delete;
  VulkanResourceManager &operator=(VulkanResourceManager &&) = delete;

  std::unique_ptr<VulkanBuffer>
  createBuffer(const BufferDescription &description);

  void destroyBuffer(const std::unique_ptr<VulkanBuffer> &buffer);

  std::unique_ptr<VulkanTexture>
  createTexture(const TextureDescription &description);

  void destroyTexture(const std::unique_ptr<VulkanTexture> &texture);

  std::unique_ptr<VulkanBuffer>
  updateBuffer(const std::unique_ptr<VulkanBuffer> &buffer,
               const BufferDescription &description);

  inline VmaAllocator getVmaAllocator() { return mAllocator; }

private:
  VmaAllocator mAllocator = VK_NULL_HANDLE;
  VulkanDeviceObject &mDevice;
  VulkanUploadContext &mUploadContext;
};

} // namespace liquid::experimental
