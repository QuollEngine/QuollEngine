#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include "renderer/TextureBinder.h"

namespace liquid {

class VulkanTextureBinder : public TextureBinder {
public:
  /**
   * @brief Create texture binder
   *
   * @param device Vulkan device
   * @param allocator Vma allocator
   * @param image Vulkan image
   * @param allocation  Vma allocation
   * @param imageView Vulkan image view
   * @param sampler Vulkan sampler
   */
  VulkanTextureBinder(VkDevice device, VmaAllocator allocator, VkImage image,
                      VmaAllocation allocation, VkImageView imageView,
                      VkSampler sampler);

  /**
   * @brief Destroy texture binder
   *
   * Destroys image, allocation, imgeview and sampler
   */
  ~VulkanTextureBinder();

  VulkanTextureBinder(const VulkanTextureBinder &rhs) = delete;
  VulkanTextureBinder(VulkanTextureBinder &&rhs) = delete;
  VulkanTextureBinder &operator=(const VulkanTextureBinder &rhs) = delete;
  VulkanTextureBinder &operator=(VulkanTextureBinder &&rhs) = delete;

  /**
   * @brief Get sampler
   *
   * @return sampler
   */
  inline VkSampler getSampler() const { return sampler; }

  /**
   * @brief Get image view
   *
   * @return Image view
   */
  inline VkImageView getImageView() const { return imageView; }

public:
  VkDevice device = nullptr;
  VmaAllocator allocator = nullptr;
  VkImage image = nullptr;
  VkImageView imageView = nullptr;
  VkSampler sampler = nullptr;
  VmaAllocation allocation = nullptr;
};

} // namespace liquid
