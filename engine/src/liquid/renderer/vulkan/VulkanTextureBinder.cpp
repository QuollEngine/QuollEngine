#include "liquid/core/Base.h"
#include "VulkanTextureBinder.h"

namespace liquid {

VulkanTextureBinder::VulkanTextureBinder(
    VkDevice device_, VmaAllocator allocator_, VkImage image_,
    VmaAllocation allocation_, VkImageView imageView_, VkSampler sampler_)
    : device(device_), allocator(allocator_), image(image_),
      allocation(allocation_), imageView(imageView_), sampler(sampler_) {}

VulkanTextureBinder::~VulkanTextureBinder() {
  if (sampler) {
    vkDestroySampler(device, sampler, nullptr);
  }

  if (imageView) {
    vkDestroyImageView(device, imageView, nullptr);
  }

  if (image) {
    vmaDestroyImage(allocator, image, allocation);
  }
}

} // namespace liquid
