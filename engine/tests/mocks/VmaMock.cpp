#include "VulkanMock.h"

VmaLibMock *VulkanTestBase::vmaLibMock = nullptr;

VMA_CALL_PRE VkResult VMA_CALL_POST vmaCreateAllocator(
    const VmaAllocatorCreateInfo *pCreateInfo, VmaAllocator *pAllocator) {
  return VulkanTestBase::vmaLibMock->vmaCreateAllocator(pCreateInfo,
                                                        pAllocator);
}

VMA_CALL_PRE void VMA_CALL_POST vmaDestroyAllocator(VmaAllocator allocator) {
  VulkanTestBase::vmaLibMock->vmaDestroyAllocator(allocator);
}

VMA_CALL_PRE VkResult VMA_CALL_POST vmaMapMemory(VmaAllocator allocator,
                                                 VmaAllocation allocation,
                                                 void **ppData) {
  return VulkanTestBase::vmaLibMock->vmaMapMemory(allocator, allocation,
                                                  ppData);
}

VMA_CALL_PRE void VMA_CALL_POST vmaUnmapMemory(VmaAllocator allocator,
                                               VmaAllocation allocation) {
  VulkanTestBase::vmaLibMock->vmaUnmapMemory(allocator, allocation);
}

VMA_CALL_PRE VkResult VMA_CALL_POST vmaCreateBuffer(
    VmaAllocator allocator, const VkBufferCreateInfo *pBufferCreateInfo,
    const VmaAllocationCreateInfo *pAllocationCreateInfo, VkBuffer *pBuffer,
    VmaAllocation *pAllocation, VmaAllocationInfo *pAllocationInfo) {
  return VulkanTestBase::vmaLibMock->vmaCreateBuffer(
      allocator, pBufferCreateInfo, pAllocationCreateInfo, pBuffer, pAllocation,
      pAllocationInfo);
}

VMA_CALL_PRE void VMA_CALL_POST vmaDestroyBuffer(VmaAllocator allocator,
                                                 VkBuffer buffer,
                                                 VmaAllocation allocation) {
  VulkanTestBase::vmaLibMock->vmaDestroyBuffer(allocator, buffer, allocation);
}

VMA_CALL_PRE VkResult VMA_CALL_POST vmaCreateImage(
    VmaAllocator allocator, const VkImageCreateInfo *pImageCreateInfo,
    const VmaAllocationCreateInfo *pAllocationCreateInfo, VkImage *pImage,
    VmaAllocation *pAllocation, VmaAllocationInfo *pAllocationInfo) {
  return VulkanTestBase::vmaLibMock->vmaCreateImage(
      allocator, pImageCreateInfo, pAllocationCreateInfo, pImage, pAllocation,
      pAllocationInfo);
}

void VMA_CALL_POST vmaDestroyImage(VmaAllocator allocator, VkImage image,
                                   VmaAllocation allocation) {
  VulkanTestBase::vmaLibMock->vmaDestroyImage(allocator, image, allocation);
}
