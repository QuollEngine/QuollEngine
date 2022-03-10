#include "liquid/core/Base.h"
#include "VulkanResourceManager.h"
#include "VulkanRenderDevice.h"

#include "liquid/renderer/vulkan/VulkanError.h"
#include "liquid/core/EngineGlobals.h"

#include "liquid/renderer/vulkan/VulkanUploadContext.h"

namespace liquid::experimental {

void VulkanResourceManager::create(VulkanRenderDevice *device) {
  mDevice = device;
  VmaAllocatorCreateInfo createInfo{};
  createInfo.instance = mDevice->getBackend().getVulkanInstance();
  createInfo.physicalDevice = mDevice->getPhysicalDevice().getVulkanDevice();
  createInfo.device = mDevice->getVulkanDevice();

  checkForVulkanError(vmaCreateAllocator(&createInfo, &mAllocator),
                      "Failed to create VMA allocator");

  mUploadContext = new VulkanUploadContext(mDevice);

  LOG_DEBUG("[Vulkan] Resource manager created");
}

void VulkanResourceManager::destroy() {
  if (mAllocator) {
    vmaDestroyAllocator(mAllocator);
    LOG_DEBUG("[Vulkan] Resource manager destroyed");
  }
}

std::unique_ptr<VulkanBuffer>
VulkanResourceManager::createBuffer(const BufferDescription &description) {
  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
  if (description.type == BufferType::Vertex) {
    bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  } else if (description.type == BufferType::Index) {
    bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  } else if (description.type == BufferType::Uniform) {
    bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  } else if (description.type == BufferType::Transfer) {
    bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
  }

  VkBufferCreateInfo createBufferInfo{};
  createBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  createBufferInfo.pNext = nullptr;
  createBufferInfo.flags = 0;
  createBufferInfo.size = description.size;
  createBufferInfo.usage = bufferUsage;

  VmaAllocationCreateInfo createAllocationInfo{};
  createAllocationInfo.flags = 0;
  createAllocationInfo.usage = memoryUsage;

  VmaAllocation allocation = VK_NULL_HANDLE;
  VkBuffer buffer = VK_NULL_HANDLE;

  checkForVulkanError(vmaCreateBuffer(mAllocator, &createBufferInfo,
                                      &createAllocationInfo, &buffer,
                                      &allocation, nullptr),
                      "Cannot create buffer");

  if (description.data) {
    void *data = nullptr;
    vmaMapMemory(mAllocator, allocation, &data);
    memcpy(data, description.data, description.size);
    vmaUnmapMemory(mAllocator, allocation);
  }

  return std::make_unique<VulkanBuffer>(description.type, description.size,
                                        buffer, allocation);
}

std::unique_ptr<VulkanBuffer>
VulkanResourceManager::updateBuffer(const std::unique_ptr<VulkanBuffer> &buffer,
                                    const BufferDescription &description) {
  LIQUID_ASSERT(buffer->getType() == description.type,
                "Cannot change the type of the buffer");

  if (buffer->getSize() != description.size) {
    destroyBuffer(buffer);
    return createBuffer(description);
  }

  if (description.data) {
    void *data = nullptr;
    vmaMapMemory(mAllocator, buffer->getAllocation(), &data);
    memcpy(data, description.data, description.size);
    vmaUnmapMemory(mAllocator, buffer->getAllocation());
  }

  return nullptr;
}

void VulkanResourceManager::destroyBuffer(
    const std::unique_ptr<VulkanBuffer> &buffer) {
  vmaDestroyBuffer(mAllocator, buffer->getBuffer(), buffer->getAllocation());
}

std::unique_ptr<VulkanTexture>
VulkanResourceManager::createTexture(const TextureDescription &description) {
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

  VmaAllocation allocation = VK_NULL_HANDLE;
  VkImage image = VK_NULL_HANDLE;
  VkImageView imageView = VK_NULL_HANDLE;
  VkSampler sampler = VK_NULL_HANDLE;

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
                                     &allocationCreateInfo, &image, &allocation,
                                     nullptr),
                      "Failed to create texture");

  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.pNext = nullptr;
  imageViewCreateInfo.flags = 0;
  imageViewCreateInfo.image = image;
  imageViewCreateInfo.viewType = imageViewType;
  imageViewCreateInfo.format = format;
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
  imageViewCreateInfo.subresourceRange.layerCount = description.layers;
  imageViewCreateInfo.subresourceRange.levelCount = 1;
  imageViewCreateInfo.subresourceRange.aspectMask = description.aspectFlags;
  checkForVulkanError(vkCreateImageView(mDevice->getVulkanDevice(),
                                        &imageViewCreateInfo, nullptr,
                                        &imageView),
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
  checkForVulkanError(vkCreateSampler(mDevice->getVulkanDevice(),
                                      &samplerCreateInfo, nullptr, &sampler),
                      "Failed to image sampler");

  if (description.data) {
    auto stagingBuffer = createBuffer(
        {BufferType::Transfer, description.size, description.data});

    mUploadContext->submit([extent, image, &stagingBuffer,
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
      imageBarrierTransfer.image = image;
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

      vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->getBuffer(), image,
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

    destroyBuffer(stagingBuffer);
  }

  return std::make_unique<VulkanTexture>(image, imageView, sampler, allocation);
}

void VulkanResourceManager::destroyTexture(
    const std::unique_ptr<VulkanTexture> &texture) {
  vkDestroySampler(mDevice->getVulkanDevice(), texture->getSampler(), nullptr);

  vkDestroyImageView(mDevice->getVulkanDevice(), texture->getImageView(),
                     nullptr);

  vmaDestroyImage(mAllocator, texture->getImage(), texture->getAllocation());
}

} // namespace liquid::experimental
