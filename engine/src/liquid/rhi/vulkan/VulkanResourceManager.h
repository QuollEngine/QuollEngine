#pragma once

#include "../BufferDescription.h"
#include "../TextureDescription.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"

namespace liquid {

class VulkanUploadContext;

} // namespace liquid

namespace liquid::experimental {

class VulkanRenderDevice;

class VulkanResourceManager {
public:
  void create(VulkanRenderDevice *device);

  void destroy();

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
  VulkanRenderDevice *mDevice = nullptr;

  VulkanUploadContext *mUploadContext = nullptr;
};

} // namespace liquid::experimental
