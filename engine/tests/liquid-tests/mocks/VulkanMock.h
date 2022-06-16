#pragma once

#include <gmock/gmock.h>

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

class VulkanLibMock {
public:
  MOCK_METHOD(VkResult, vkCreateDescriptorSetLayout,
              (VkDevice, const VkDescriptorSetLayoutCreateInfo *,
               const VkAllocationCallbacks *, VkDescriptorSetLayout *));

  MOCK_METHOD(VkResult, vkCreateDescriptorPool,
              (VkDevice, const VkDescriptorPoolCreateInfo *,
               const VkAllocationCallbacks *, VkDescriptorPool *));

  MOCK_METHOD(VkResult, vkAllocateDescriptorSets,
              (VkDevice, const VkDescriptorSetAllocateInfo *,
               VkDescriptorSet *));

  MOCK_METHOD(void, vkUpdateDescriptorSets,
              (VkDevice, uint32_t, const VkWriteDescriptorSet *, uint32_t,
               const VkCopyDescriptorSet *));

  MOCK_METHOD(void, vkDestroyDescriptorSetLayout,
              (VkDevice, VkDescriptorSetLayout, const VkAllocationCallbacks *));
  MOCK_METHOD(void, vkDestroyDescriptorPool,
              (VkDevice, VkDescriptorPool, const VkAllocationCallbacks *));

  MOCK_METHOD(VkResult, vkCreateRenderPass,
              (VkDevice, const VkRenderPassCreateInfo *,
               const VkAllocationCallbacks *, VkRenderPass *));
  MOCK_METHOD(VkResult, vkCreateFramebuffer,
              (VkDevice, const VkFramebufferCreateInfo *,
               const VkAllocationCallbacks *, VkFramebuffer *));

  MOCK_METHOD(void, vkDestroyFramebuffer,
              (VkDevice, VkFramebuffer, const VkAllocationCallbacks *));

  MOCK_METHOD(void, vkDestroyRenderPass,
              (VkDevice, VkRenderPass, const VkAllocationCallbacks *));

  MOCK_METHOD(void, vkCmdBeginRenderPass,
              (VkCommandBuffer, const VkRenderPassBeginInfo *,
               VkSubpassContents));
  MOCK_METHOD(void, vkCmdEndRenderPass, (VkCommandBuffer));

  MOCK_METHOD(VkResult, vkCreateImageView,
              (VkDevice, const VkImageViewCreateInfo *,
               const VkAllocationCallbacks *, VkImageView *));

  MOCK_METHOD(VkResult, vkCreateSampler,
              (VkDevice, const VkSamplerCreateInfo *,
               const VkAllocationCallbacks *, VkSampler *));
};

class VmaLibMock {
public:
  MOCK_METHOD(VkResult, vmaCreateAllocator,
              (const VmaAllocatorCreateInfo *, VmaAllocator *));

  MOCK_METHOD(void, vmaDestroyAllocator, (VmaAllocator));

  MOCK_METHOD(VkResult, vmaMapMemory, (VmaAllocator, VmaAllocation, void **));

  MOCK_METHOD(void, vmaUnmapMemory, (VmaAllocator, VmaAllocation));

  MOCK_METHOD(VkResult, vmaCreateBuffer,
              (VmaAllocator, const VkBufferCreateInfo *,
               const VmaAllocationCreateInfo *, VkBuffer *, VmaAllocation *,
               VmaAllocationInfo *));

  MOCK_METHOD(void, vmaDestroyBuffer, (VmaAllocator, VkBuffer, VmaAllocation));

  MOCK_METHOD(VkResult, vmaCreateImage,
              (VmaAllocator, const VkImageCreateInfo *,
               const VmaAllocationCreateInfo *, VkImage *pImage,
               VmaAllocation *, VmaAllocationInfo *));

  MOCK_METHOD(void, vmaDestroyImage,
              (VmaAllocator allocator, VkImage image,
               VmaAllocation allocation));
};

class VulkanTestBase : public ::testing::Test {
public:
  VulkanTestBase() {
    vulkanLibMock = new ::testing::NiceMock<VulkanLibMock>;
    vmaLibMock = new ::testing::NiceMock<VmaLibMock>;
  }

  ~VulkanTestBase() {
    delete vulkanLibMock;
    vulkanLibMock = nullptr;

    delete vmaLibMock;
    vmaLibMock = nullptr;
  }

public:
  static VulkanLibMock *vulkanLibMock;
  static VmaLibMock *vmaLibMock;
};
