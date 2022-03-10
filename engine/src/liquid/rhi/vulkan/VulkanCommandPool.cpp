#include "liquid/core/Base.h"

#include <vulkan/vulkan.hpp>
#include "VulkanRenderDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"

#include "liquid/renderer/vulkan/VulkanError.h"
#include "liquid/core/EngineGlobals.h"

namespace liquid::experimental {

VulkanCommandPool::VulkanCommandPool(VulkanDeviceObject &device,
                                     uint32_t queueFamilyIndex,
                                     const VulkanResourceRegistry &registry,
                                     VulkanDescriptorManager &descriptorManager)
    : mDevice(device), mRegistry(registry),
      mDescriptorManager(descriptorManager) {
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndex;

  checkForVulkanError(
      vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool),
      "Failed to create command pool");

  LOG_DEBUG("[Vulkan] Command pool created");
}

VulkanCommandPool::~VulkanCommandPool() {
  if (mCommandPool) {
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    LOG_DEBUG("[Vulkan] Command pool destroyed");
  }
}

std::vector<RenderCommandList>
VulkanCommandPool::createCommandLists(uint32_t count) {

  std::vector<VkCommandBuffer> commandBuffers(count);
  std::vector<RenderCommandList> renderCommandLists(count);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = mCommandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = count;

  checkForVulkanError(
      vkAllocateCommandBuffers(mDevice, &allocInfo, commandBuffers.data()),
      "Failed to allocate command buffers");

  for (size_t i = 0; i < count; ++i) {

    renderCommandLists.at(i) =
        std::move(RenderCommandList(new VulkanCommandBuffer(
            commandBuffers.at(i), mRegistry, mDescriptorManager)));
  }

  LOG_DEBUG("[Vulkan] Command buffers allocated");

  return std::move(renderCommandLists);
}

} // namespace liquid::experimental
