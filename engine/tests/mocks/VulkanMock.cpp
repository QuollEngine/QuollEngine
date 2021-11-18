#include "core/Base.h"
#include <vulkan/vulkan.hpp>
#include "VulkanMock.h"

#define VK_NO_IMPL(fn)                                                         \
  VkResult fn { return VK_ERROR_UNKNOWN; }

#define VK_NO_IMPL_VOID(fn)                                                    \
  void fn {}

VulkanLibMock *VulkanTestBase::vulkanLibMock = nullptr;

PFN_vkVoidFunction vkGetDeviceProcAddr(VkDevice device, const char *pName) {
  return nullptr;
}

PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance instance,
                                         const char *pName) {
  return nullptr;
}

VK_NO_IMPL(vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain,
                                 uint64_t timeout, VkSemaphore semaphore,
                                 VkFence fence, uint32_t *pImageIndex));

VK_NO_IMPL(vkAllocateCommandBuffers(
    VkDevice device, const VkCommandBufferAllocateInfo *pAllocateInfo,
    VkCommandBuffer *pCommandBuffers));

VkResult
vkAllocateDescriptorSets(VkDevice device,
                         const VkDescriptorSetAllocateInfo *pAllocateInfo,
                         VkDescriptorSet *pDescriptorSets) {
  return VulkanTestBase::vulkanLibMock->vkAllocateDescriptorSets(
      device, pAllocateInfo, pDescriptorSets);
}

VK_NO_IMPL(vkAllocateMemory(VkDevice device,
                            const VkMemoryAllocateInfo *pAllocateInfo,
                            const VkAllocationCallbacks *pAllocator,
                            VkDeviceMemory *pMemory));

VK_NO_IMPL(vkBeginCommandBuffer(VkCommandBuffer commandBuffer,
                                const VkCommandBufferBeginInfo *pBeginInfo));

VK_NO_IMPL(vkBindBufferMemory(VkDevice device, VkBuffer buffer,
                              VkDeviceMemory memory,
                              VkDeviceSize memoryOffset));

VK_NO_IMPL(vkBindImageMemory(VkDevice device, VkImage image,
                             VkDeviceMemory memory, VkDeviceSize memoryOffset));

void vkCmdBeginRenderPass(VkCommandBuffer commandBuffer,
                          const VkRenderPassBeginInfo *pRenderPassBegin,
                          VkSubpassContents contents) {
  VulkanTestBase::vulkanLibMock->vkCmdBeginRenderPass(
      commandBuffer, pRenderPassBegin, contents);
}

VK_NO_IMPL_VOID(vkCmdBindDescriptorSets(
    VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount,
    const VkDescriptorSet *pDescriptorSets, uint32_t dynamicOffsetCount,
    const uint32_t *pDynamicOffsets));

VK_NO_IMPL_VOID(vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer,
                                     VkBuffer buffer, VkDeviceSize offset,
                                     VkIndexType indexType));
VK_NO_IMPL_VOID(vkCmdBindPipeline(VkCommandBuffer commandBuffer,
                                  VkPipelineBindPoint pipelineBindPoint,
                                  VkPipeline pipeline));

VK_NO_IMPL_VOID(vkCmdCopyBuffer(VkCommandBuffer commandBuffer,
                                VkBuffer srcBuffer, VkBuffer dstBuffer,
                                uint32_t regionCount,
                                const VkBufferCopy *pRegions));

VK_NO_IMPL_VOID(vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer,
                                       VkBuffer srcBuffer, VkImage dstImage,
                                       VkImageLayout dstImageLayout,
                                       uint32_t regionCount,
                                       const VkBufferImageCopy *pRegions));

VK_NO_IMPL_VOID(vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount,
                          uint32_t instanceCount, uint32_t firstVertex,
                          uint32_t firstIndex));

VK_NO_IMPL_VOID(vkCmdDrawIndexed(VkCommandBuffer commandBuffer,
                                 uint32_t indexCount, uint32_t instanceCount,
                                 uint32_t firstIndex, int32_t vertexOffset,
                                 uint32_t firstInstance));

void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
  VulkanTestBase::vulkanLibMock->vkCmdEndRenderPass(commandBuffer);
}

VK_NO_IMPL_VOID(vkCmdPipelineBarrier(
    VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,
    uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier *pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier *pImageMemoryBarriers));

VK_NO_IMPL_VOID(vkCmdPushConstants(VkCommandBuffer commandBuffer,
                                   VkPipelineLayout layout,
                                   VkShaderStageFlags stageFlags,
                                   uint32_t offset, uint32_t size,
                                   const void *pValues));

VK_NO_IMPL_VOID(vkCmdBindVertexBuffers(
    VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount,
    const VkBuffer *pBuffers, const VkDeviceSize *pOffsets));

VK_NO_IMPL_VOID(vkCmdSetScissor(VkCommandBuffer commandBuffer,
                                uint32_t firstScissor, uint32_t scissorCount,
                                const VkRect2D *pScissors));

VK_NO_IMPL_VOID(vkCmdSetViewport(VkCommandBuffer commandBuffer,
                                 uint32_t firstViewport, uint32_t viewportCount,
                                 const VkViewport *pViewports));

VK_NO_IMPL(vkCreateBuffer(VkDevice device,
                          const VkBufferCreateInfo *pCreateInfo,
                          const VkAllocationCallbacks *pAllocator,
                          VkBuffer *pBuffer));

VK_NO_IMPL(vkCreateCommandPool(VkDevice device,
                               const VkCommandPoolCreateInfo *pCreateInfo,
                               const VkAllocationCallbacks *pAllocator,
                               VkCommandPool *pCommandPool));

VkResult vkCreateDescriptorPool(VkDevice device,
                                const VkDescriptorPoolCreateInfo *pCreateInfo,
                                const VkAllocationCallbacks *pAllocator,
                                VkDescriptorPool *pDescriptorPool) {
  return VulkanTestBase::vulkanLibMock->vkCreateDescriptorPool(
      device, pCreateInfo, pAllocator, pDescriptorPool);
}

VkResult
vkCreateDescriptorSetLayout(VkDevice device,
                            const VkDescriptorSetLayoutCreateInfo *pCreateInfo,
                            const VkAllocationCallbacks *pAllocator,
                            VkDescriptorSetLayout *pSetLayout) {
  return VulkanTestBase::vulkanLibMock->vkCreateDescriptorSetLayout(
      device, pCreateInfo, pAllocator, pSetLayout);
}

VK_NO_IMPL(vkCreateDevice(VkPhysicalDevice physicalDevice,
                          const VkDeviceCreateInfo *pCreateInfo,
                          const VkAllocationCallbacks *pAllocator,
                          VkDevice *pDevice));

VK_NO_IMPL(vkCreateFence(VkDevice device, const VkFenceCreateInfo *pCreateInfo,
                         const VkAllocationCallbacks *pAllocator,
                         VkFence *pFence));

VkResult vkCreateFramebuffer(VkDevice device,
                             const VkFramebufferCreateInfo *pCreateInfo,
                             const VkAllocationCallbacks *pAllocator,
                             VkFramebuffer *pFramebuffer) {
  return VulkanTestBase::vulkanLibMock->vkCreateFramebuffer(
      device, pCreateInfo, pAllocator, pFramebuffer);
}

VK_NO_IMPL(vkCreateGraphicsPipelines(
    VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo *pCreateInfos,
    const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines));

VK_NO_IMPL(vkCreateImage(VkDevice device, const VkImageCreateInfo *pCreateInfo,
                         const VkAllocationCallbacks *pAllocator,
                         VkImage *pImage));

VkResult vkCreateImageView(VkDevice device,
                           const VkImageViewCreateInfo *pCreateInfo,
                           const VkAllocationCallbacks *pAllocator,
                           VkImageView *pView) {
  return VulkanTestBase::vulkanLibMock->vkCreateImageView(device, pCreateInfo,
                                                          pAllocator, pView);
}

VK_NO_IMPL(vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo,
                            const VkAllocationCallbacks *pAllocator,
                            VkInstance *pInstance));

VK_NO_IMPL(vkCreatePipelineLayout(VkDevice device,
                                  const VkPipelineLayoutCreateInfo *pCreateInfo,
                                  const VkAllocationCallbacks *pAllocator,
                                  VkPipelineLayout *pPipelineLayout));

VkResult vkCreateRenderPass(VkDevice device,
                            const VkRenderPassCreateInfo *pCreateInfo,
                            const VkAllocationCallbacks *pAllocator,
                            VkRenderPass *pRenderPass) {
  return VulkanTestBase::vulkanLibMock->vkCreateRenderPass(
      device, pCreateInfo, pAllocator, pRenderPass);
}

VkResult vkCreateSampler(VkDevice device,
                         const VkSamplerCreateInfo *pCreateInfo,
                         const VkAllocationCallbacks *pAllocator,
                         VkSampler *pSampler) {
  return VulkanTestBase::vulkanLibMock->vkCreateSampler(device, pCreateInfo,
                                                        pAllocator, pSampler);
}

VK_NO_IMPL(vkCreateSemaphore(VkDevice device,
                             const VkSemaphoreCreateInfo *pCreateInfo,
                             const VkAllocationCallbacks *pAllocator,
                             VkSemaphore *pSemaphore));

VK_NO_IMPL(vkCreateShaderModule(VkDevice device,
                                const VkShaderModuleCreateInfo *pCreateInfo,
                                const VkAllocationCallbacks *pAllocator,
                                VkShaderModule *pShaderModule));
VK_NO_IMPL(vkCreateSwapchainKHR(VkDevice device,
                                const VkSwapchainCreateInfoKHR *pCreateInfo,
                                const VkAllocationCallbacks *pAllocator,
                                VkSwapchainKHR *pSwapchain));

VK_NO_IMPL_VOID(vkDestroyBuffer(VkDevice device, VkBuffer buffer,
                                const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool,
                                     const VkAllocationCallbacks *pAllocator));

void vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool,
                             const VkAllocationCallbacks *pAllocator) {
  VulkanTestBase::vulkanLibMock->vkDestroyDescriptorPool(device, descriptorPool,
                                                         pAllocator);
}

void vkDestroyDescriptorSetLayout(VkDevice device,
                                  VkDescriptorSetLayout descriptorSetLayout,
                                  const VkAllocationCallbacks *pAllocator) {
  VulkanTestBase::vulkanLibMock->vkDestroyDescriptorSetLayout(
      device, descriptorSetLayout, pAllocator);
}

VK_NO_IMPL_VOID(vkDestroyDevice(VkDevice device,
                                const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(vkDestroyFence(VkDevice device, VkFence fence,
                               const VkAllocationCallbacks *pAllocator));

void vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer,
                          const VkAllocationCallbacks *pAllocator) {
  VulkanTestBase::vulkanLibMock->vkDestroyFramebuffer(device, framebuffer,
                                                      pAllocator);
}

VK_NO_IMPL_VOID(vkDestroyImage(VkDevice device, VkImage image,
                               const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(vkDestroyImageView(VkDevice device, VkImageView imageView,
                                   const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(vkDestroyInstance(VkInstance instance,
                                  const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(vkDestroyPipeline(VkDevice device, VkPipeline pipeline,
                                  const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(
    vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache,
                           const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(
    vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout,
                            const VkAllocationCallbacks *pAllocator));

void vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass,
                         const VkAllocationCallbacks *pAllocator) {
  VulkanTestBase::vulkanLibMock->vkDestroyRenderPass(device, renderPass,
                                                     pAllocator);
}

VK_NO_IMPL_VOID(vkDestroySampler(VkDevice device, VkSampler sampler,
                                 const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(vkDestroySemaphore(VkDevice device, VkSemaphore semaphore,
                                   const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(vkDestroyShaderModule(VkDevice device,
                                      VkShaderModule shaderModule,
                                      const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                    const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL_VOID(vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain,
                                      const VkAllocationCallbacks *pAllocator));

VK_NO_IMPL(vkDeviceWaitIdle(VkDevice device));

VK_NO_IMPL(vkEndCommandBuffer(VkCommandBuffer commandBuffer));

VK_NO_IMPL(vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char *pLayerName,
    uint32_t *pPropertyCount, VkExtensionProperties *pProperties));

VK_NO_IMPL(vkEnumerateInstanceExtensionProperties(
    const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties));

VK_NO_IMPL(vkEnumerateInstanceLayerProperties(uint32_t *pPropertyCount,
                                              VkLayerProperties *pProperties));

VK_NO_IMPL(vkEnumeratePhysicalDevices(VkInstance instance,
                                      uint32_t *pPhysicalDeviceCount,
                                      VkPhysicalDevice *pPhysicalDevices));

VK_NO_IMPL(vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                                     const VkMappedMemoryRange *pMemoryRanges));

VK_NO_IMPL_VOID(vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool,
                                     uint32_t commandBufferCount,
                                     const VkCommandBuffer *pCommandBuffers));

VK_NO_IMPL(vkFreeDescriptorSets(VkDevice device,
                                VkDescriptorPool descriptorPool,
                                uint32_t descriptorSetCount,
                                const VkDescriptorSet *pDescriptorSets));

VK_NO_IMPL_VOID(vkFreeMemory(VkDevice device, VkDeviceMemory memory,
                             const VkAllocationCallbacks *pAllocator));
VK_NO_IMPL_VOID(
    vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer,
                                  VkMemoryRequirements *pMemoryRequirements));

VK_NO_IMPL_VOID(vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex,
                                 uint32_t queueIndex, VkQueue *pQueue));

VK_NO_IMPL_VOID(vkGetImageMemoryRequirements(
    VkDevice device, VkImage image, VkMemoryRequirements *pMemoryRequirements));

VK_NO_IMPL_VOID(vkGetPhysicalDeviceFeatures(
    VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures *pFeatures));

VK_NO_IMPL_VOID(vkGetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceMemoryProperties *pMemoryProperties));

VK_NO_IMPL_VOID(vkGetPhysicalDeviceProperties(
    VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *pProperties));

VK_NO_IMPL_VOID(vkGetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice physicalDevice, uint32_t *pQueueFamilyPropertyCount,
    VkQueueFamilyProperties *pQueueFamilyProperties));

VK_NO_IMPL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR *pSurfaceCapabilities));

VK_NO_IMPL(vkGetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
    uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats));

VK_NO_IMPL(vkGetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
    uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes));

VK_NO_IMPL(vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice,
                                                uint32_t queueFamilyIndex,
                                                VkSurfaceKHR surface,
                                                VkBool32 *pSupported));

VK_NO_IMPL(vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain,
                                   uint32_t *pSwapchainImageCount,
                                   VkImage *pSwapchainImages));

VK_NO_IMPL(
    vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                                   const VkMappedMemoryRange *pMemoryRanges));

VK_NO_IMPL(vkMapMemory(VkDevice device, VkDeviceMemory memory,
                       VkDeviceSize offset, VkDeviceSize size,
                       VkMemoryMapFlags flags, void **ppData));

VK_NO_IMPL(vkQueuePresentKHR(VkQueue queue,
                             const VkPresentInfoKHR *pPresentInfo));

VK_NO_IMPL(vkQueueSubmit(VkQueue queue, uint32_t submitCount,
                         const VkSubmitInfo *pSubmits, VkFence fence));

VK_NO_IMPL(vkResetCommandBuffer(VkCommandBuffer commandBuffer,
                                VkCommandBufferResetFlags flags));

VK_NO_IMPL(vkResetCommandPool(VkDevice device, VkCommandPool commandPool,
                              VkCommandPoolResetFlags flags));

VK_NO_IMPL(vkResetFences(VkDevice device, uint32_t fenceCount,
                         const VkFence *pFences));

VK_NO_IMPL_VOID(vkUnmapMemory(VkDevice device, VkDeviceMemory memory));

void vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount,
                            const VkWriteDescriptorSet *pDescriptorWrites,
                            uint32_t descriptorCopyCount,
                            const VkCopyDescriptorSet *pDescriptorCopies) {
  VulkanTestBase::vulkanLibMock->vkUpdateDescriptorSets(
      device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount,
      pDescriptorCopies);
}

VK_NO_IMPL(vkWaitForFences(VkDevice device, uint32_t fenceCount,
                           const VkFence *pFences, VkBool32 waitAll,
                           uint64_t timeout));
