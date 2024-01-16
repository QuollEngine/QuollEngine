#pragma once

#include "VulkanHeaders.h"
#include <vk_mem_alloc.h>
#include "VulkanDeviceObject.h"
#include "VulkanUploadContext.h"
#include "VulkanResourceAllocator.h"
#include "VulkanResourceRegistry.h"

#include "quoll/rhi/TextureDescription.h"
#include "quoll/rhi/TextureViewDescription.h"

namespace quoll::rhi {

class VulkanTexture {
public:
  VulkanTexture(VkImage image, VkImageView imageView, VkSampler sampler,
                VkFormat format, VulkanResourceAllocator &allocator,
                VulkanDeviceObject &device);

  VulkanTexture(const TextureDescription &description,
                VulkanResourceAllocator &allocator, VulkanDeviceObject &device);

  VulkanTexture(const TextureViewDescription &description,
                VulkanResourceRegistry &registry,
                VulkanResourceAllocator &allocator, VulkanDeviceObject &device);

  ~VulkanTexture();

  VulkanTexture(const VulkanTexture &) = delete;
  VulkanTexture &operator=(const VulkanTexture &) = delete;
  VulkanTexture(VulkanTexture &&) = delete;
  VulkanTexture &operator=(VulkanTexture &&) = delete;

  inline VkImage getImage() const { return mImage; }

  inline VkImageView getImageView() const { return mImageView; }

  inline VmaAllocation getAllocation() const { return mAllocation; }

  inline VkFormat getFormat() const { return mFormat; }

  inline VkImageAspectFlags getImageAspectFlags() const { return mAspectFlags; }

  inline const TextureDescription &getDescription() const {
    return mDescription;
  }

private:
  VkFormat mFormat = VK_FORMAT_MAX_ENUM;
  VkImage mImage = VK_NULL_HANDLE;
  VkImageView mImageView = VK_NULL_HANDLE;
  VmaAllocation mAllocation = VK_NULL_HANDLE;
  VkImageAspectFlags mAspectFlags = VK_IMAGE_ASPECT_NONE;
  VulkanResourceAllocator &mAllocator;
  VulkanDeviceObject &mDevice;
  TextureDescription mDescription;
  TextureViewDescription mViewDescription;
};

} // namespace quoll::rhi
