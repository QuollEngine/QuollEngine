#include "liquid/core/Base.h"

#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanError.h"

namespace liquid::experimental {

VulkanTexture::VulkanTexture(VkImage image, VkImageView imageView,
                             VkSampler sampler, VkFormat format,
                             VulkanResourceAllocator &allocator,
                             VulkanDeviceObject &device)
    : mAllocator(allocator), mDevice(device), mImage(image),
      mImageView(imageView), mSampler(sampler), mFormat(format) {}

VulkanTexture::VulkanTexture(const TextureDescription &description,
                             VulkanResourceAllocator &allocator,
                             VulkanDeviceObject &device,
                             VulkanUploadContext &uploadContext)
    : mAllocator(allocator), mDevice(device),
      mFormat(static_cast<VkFormat>(description.format)) {
  LIQUID_ASSERT(
      description.type == TextureType::Cubemap ? description.layers == 6 : true,
      "Cubemap must have 6 layers");

  uint32_t imageFlags = description.type == TextureType::Cubemap
                            ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
                            : 0;

  VkFormat format = static_cast<VkFormat>(description.format);
  VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;

  if (description.type == TextureType::Cubemap) {
    imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
  } else if (description.layers > 1) {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  } else {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D;
  }

  VkExtent3D extent{};
  extent.width = description.width;
  extent.height = description.height;
  extent.depth = description.depth;

  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = imageFlags;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = format;
  imageCreateInfo.extent = extent;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = description.layers;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.usage = description.usageFlags;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
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
  imageViewCreateInfo.format = format;
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
  imageViewCreateInfo.subresourceRange.layerCount = description.layers;
  imageViewCreateInfo.subresourceRange.levelCount = 1;
  imageViewCreateInfo.subresourceRange.aspectMask = description.aspectFlags;
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

  if (description.data) {
    VulkanBuffer stagingBuffer(
        {BufferType::Transfer, description.size, description.data}, mAllocator);

    uploadContext.submit([extent, this, &stagingBuffer,
                          &description](VkCommandBuffer commandBuffer) {
      VkImageSubresourceRange range{};
      range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      range.baseMipLevel = 0;
      range.levelCount = 1;
      range.baseArrayLayer = 0;
      range.layerCount = description.layers;

      VkImageMemoryBarrier imageBarrierTransfer{};
      imageBarrierTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageBarrierTransfer.pNext = nullptr;
      imageBarrierTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageBarrierTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imageBarrierTransfer.image = mImage;
      imageBarrierTransfer.subresourceRange = range;
      imageBarrierTransfer.srcAccessMask = 0;
      imageBarrierTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                           VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                           nullptr, 1, &imageBarrierTransfer);

      VkBufferImageCopy copyRegion{};
      copyRegion.bufferImageHeight = 0;
      copyRegion.bufferOffset = 0;
      copyRegion.bufferRowLength = 0;
      copyRegion.imageExtent = extent;
      copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      copyRegion.imageSubresource.baseArrayLayer = 0;
      copyRegion.imageSubresource.layerCount = description.layers;
      copyRegion.imageSubresource.mipLevel = 0;

      vkCmdCopyBufferToImage(commandBuffer, stagingBuffer.getBuffer(), mImage,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                             &copyRegion);

      VkImageMemoryBarrier imageBarrierReadable = imageBarrierTransfer;
      imageBarrierReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      imageBarrierReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageBarrierReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imageBarrierReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                           VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                           0, nullptr, 1, &imageBarrierReadable);
    });
  }
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

} // namespace liquid::experimental
