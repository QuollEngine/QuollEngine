#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include "VulkanDeviceObject.h"
#include "VulkanUploadContext.h"
#include "VulkanResourceAllocator.h"

#include "liquid/rhi/TextureDescription.h"

namespace liquid::experimental {

class VulkanTexture {
public:
  /**
   * @brief Create Vulkan texture
   *
   * @param description Texture description
   * @param allocator Vma allocator
   * @param device Vulkan device
   * @param uploadContext Upload context
   */
  VulkanTexture(const TextureDescription &description,
                VulkanResourceAllocator &allocator, VulkanDeviceObject &device,
                VulkanUploadContext &uploadContext);

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

private:
  VkImage mImage = VK_NULL_HANDLE;
  VkImageView mImageView = VK_NULL_HANDLE;
  VkSampler mSampler = VK_NULL_HANDLE;
  VmaAllocation mAllocation = VK_NULL_HANDLE;
  VulkanResourceAllocator &mAllocator;
  VulkanDeviceObject &mDevice;
};

} // namespace liquid::experimental
