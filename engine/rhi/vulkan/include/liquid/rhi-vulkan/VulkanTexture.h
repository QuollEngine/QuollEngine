#pragma once

#include "VulkanHeaders.h"
#include <vma/vk_mem_alloc.h>
#include "VulkanDeviceObject.h"
#include "VulkanUploadContext.h"
#include "VulkanResourceAllocator.h"
#include "VulkanResourceRegistry.h"

#include "liquid/rhi/TextureDescription.h"
#include "liquid/rhi/TextureViewDescription.h"

namespace liquid::rhi {

/**
 * @brief Vulkan texture
 */
class VulkanTexture {
public:
  /**
   * @brief Create Vulkan texture
   *
   * Create texture from available resources
   * @param image Vulkan image
   * @param imageView Vulkan image view
   * @param sampler Vulkan sampler
   * @param format Vulkan format
   * @param allocator Vulkan Resource allocator
   * @param device Vulkan device
   */
  VulkanTexture(VkImage image, VkImageView imageView, VkSampler sampler,
                VkFormat format, VulkanResourceAllocator &allocator,
                VulkanDeviceObject &device);

  /**
   * @brief Create Vulkan texture
   *
   * @param description Texture description
   * @param allocator Vma allocator
   * @param device Vulkan device
   */
  VulkanTexture(const TextureDescription &description,
                VulkanResourceAllocator &allocator, VulkanDeviceObject &device);

  /**
   * @brief Create Vulkan texture view
   *
   * @param description Texture description
   * @param registry Vulkan resource registry
   * @param allocator Vulkan resource allocator
   * @param device Vulkan device
   */
  VulkanTexture(const TextureViewDescription &description,
                VulkanResourceRegistry &registry,
                VulkanResourceAllocator &allocator, VulkanDeviceObject &device);

  /**
   * @brief Destroy texture
   */
  ~VulkanTexture();

  VulkanTexture(const VulkanTexture &) = delete;
  VulkanTexture &operator=(const VulkanTexture &) = delete;
  VulkanTexture(VulkanTexture &&) = delete;
  VulkanTexture &operator=(VulkanTexture &&) = delete;

  /**
   * @brief Get Vulkan image
   *
   * @return Vulkan image
   */
  inline VkImage getImage() const { return mImage; }

  /**
   * @brief Get Vulkan image view
   *
   * @return Vulkan image view
   */
  inline VkImageView getImageView() const { return mImageView; }

  /**
   * @brief Get Vulkan sampler
   *
   * @return Vulkan sampler
   */
  inline VkSampler getSampler() const { return mSampler; }

  /**
   * @brief Get Vulkan allocation
   *
   * @return Vulkan allocation
   */
  inline VmaAllocation getAllocation() const { return mAllocation; }

  /**
   * @brief Get Vulkan format
   *
   * @return Vulkan format
   */
  inline VkFormat getFormat() const { return mFormat; }

  /**
   * @brief Get image aspect flags
   *
   * @return Image aspect flags
   */
  inline VkImageAspectFlags getImageAspectFlags() const { return mAspectFlags; }

  /**
   * @brief Get description
   *
   * @return Description
   */
  inline const TextureDescription &getDescription() const {
    return mDescription;
  }

private:
  VkFormat mFormat = VK_FORMAT_MAX_ENUM;
  VkImage mImage = VK_NULL_HANDLE;
  VkImageView mImageView = VK_NULL_HANDLE;
  VkSampler mSampler = VK_NULL_HANDLE;
  VmaAllocation mAllocation = VK_NULL_HANDLE;
  VkImageAspectFlags mAspectFlags = VK_IMAGE_ASPECT_NONE;
  VulkanResourceAllocator &mAllocator;
  VulkanDeviceObject &mDevice;
  TextureDescription mDescription;
  TextureViewDescription mViewDescription;
};

} // namespace liquid::rhi
