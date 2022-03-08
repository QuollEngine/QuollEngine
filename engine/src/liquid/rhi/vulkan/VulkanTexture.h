#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

namespace liquid::experimental {

class VulkanTexture {
public:
  /**
   * @brief Create Vulkan texture
   * @param image Vulkan image
   * @param imageView Vulkan image view
   * @param sampler Vulkan sampler
   * @param allocation Vulkan allocation
   */
  VulkanTexture(VkImage image, VkImageView imageView, VkSampler sampler,
                VmaAllocation allocation);

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
};

} // namespace liquid::experimental
