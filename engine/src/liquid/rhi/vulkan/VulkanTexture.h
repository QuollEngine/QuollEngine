#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include "VulkanDeviceObject.h"
#include "VulkanUploadContext.h"
#include "VulkanResourceAllocator.h"

#include "liquid/rhi/TextureDescription.h"

namespace liquid::rhi {

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
   * @param resourceAllocator Resource allocator
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
   * @param uploadContext Upload context
   * @param swapchainExtent Swapchain extent
   */
  VulkanTexture(const TextureDescription &description,
                VulkanResourceAllocator &allocator, VulkanDeviceObject &device,
                VulkanUploadContext &uploadContext,
                const glm::uvec2 &swapchainExtent);

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
   */
  inline VkFormat getFormat() const { return mFormat; }

  /**
   * @brief Check if texture resizes with swapchain
   *
   * @retval true Texture resizes with swapchain
   * @retval false Texture does not resize with swapchain
   */
  inline bool isSwapchainRelative() const {
    return mDescription.sizeMethod == TextureSizeMethod::SwapchainRatio;
  }

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
  VulkanResourceAllocator &mAllocator;
  VulkanDeviceObject &mDevice;
  TextureDescription mDescription;
};

} // namespace liquid::rhi
