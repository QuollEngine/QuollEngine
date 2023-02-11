#include "liquid/core/Base.h"

#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanError.h"
#include "VulkanMapping.h"

namespace liquid::rhi {

VulkanTexture::VulkanTexture(VkImage image, VkImageView imageView,
                             VkSampler sampler, VkFormat format,
                             VulkanResourceAllocator &allocator,
                             VulkanDeviceObject &device)
    : mAllocator(allocator), mDevice(device), mImage(image),
      mImageView(imageView), mSampler(sampler), mFormat(format) {

  // Note: This constructor is ONLY used
  // for defining swapchain; so, its usage
  // will always be Color
  mDescription.usage = TextureUsage::Color;
}

VulkanTexture::VulkanTexture(const TextureDescription &description,
                             VulkanResourceAllocator &allocator,
                             VulkanDeviceObject &device,
                             VulkanUploadContext &uploadContext,
                             const glm::uvec2 &swapchainExtent)
    : mAllocator(allocator), mDevice(device),
      mFormat(VulkanMapping::getFormat(description.format)),
      mDescription(description) {
  LIQUID_ASSERT(
      description.type == TextureType::Cubemap ? description.layers == 6 : true,
      "Cubemap must have 6 layers");

  uint32_t imageFlags = description.type == TextureType::Cubemap
                            ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
                            : 0;

  VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;

  if (description.type == TextureType::Cubemap) {
    imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
  } else if (description.layers > 1) {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  } else {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D;
  }

  static constexpr uint32_t HundredPercent = 100;

  VkExtent3D extent{};
  if (isFramebufferRelative()) {
    extent.width = description.width * swapchainExtent.x / HundredPercent;
    extent.height = description.height * swapchainExtent.y / HundredPercent;
  } else {
    extent.width = description.width;
    extent.height = description.height;
  }
  extent.depth = description.depth;

  VkImageUsageFlags usageFlags = 0;

  if (BitwiseEnumContains(description.usage, TextureUsage::Color)) {
    usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    mAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  if (BitwiseEnumContains(description.usage, TextureUsage::Depth)) {
    usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    mAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
  }

  if (BitwiseEnumContains(description.usage, TextureUsage::Sampled)) {
    usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
  }

  if (BitwiseEnumContains(description.usage, TextureUsage::TransferSource)) {
    usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  if (BitwiseEnumContains(description.usage,
                          TextureUsage::TransferDestination)) {
    usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = imageFlags;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = mFormat;
  imageCreateInfo.extent = extent;
  imageCreateInfo.mipLevels = description.levels;
  imageCreateInfo.arrayLayers = description.layers;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.usage = usageFlags;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
  allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
  checkForVulkanError(vmaCreateImage(mAllocator, &imageCreateInfo,
                                     &allocationCreateInfo, &mImage,
                                     &mAllocation, nullptr),
                      "Failed to create texture");

  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.pNext = nullptr;
  imageViewCreateInfo.flags = 0;
  imageViewCreateInfo.image = mImage;
  imageViewCreateInfo.viewType = imageViewType;
  imageViewCreateInfo.format = mFormat;
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
  imageViewCreateInfo.subresourceRange.layerCount = description.layers;
  imageViewCreateInfo.subresourceRange.levelCount = description.levels;
  imageViewCreateInfo.subresourceRange.aspectMask = mAspectFlags;
  checkForVulkanError(
      vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &mImageView),
      "Failed to create image view");

  VkSamplerCreateInfo samplerCreateInfo{};
  samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerCreateInfo.pNext = nullptr;
  samplerCreateInfo.flags = 0;
  samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
  samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
  checkForVulkanError(
      vkCreateSampler(mDevice, &samplerCreateInfo, nullptr, &mSampler),
      "Failed to image sampler");
}

VulkanTexture::~VulkanTexture() {
  if (mSampler) {
    vkDestroySampler(mDevice, mSampler, nullptr);
  }

  if (mImageView) {
    vkDestroyImageView(mDevice, mImageView, nullptr);
  }

  if (mAllocation && mImage) {
    vmaDestroyImage(mAllocator, mImage, mAllocation);
  }
}

} // namespace liquid::rhi
