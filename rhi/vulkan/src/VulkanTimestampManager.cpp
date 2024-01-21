#include "quoll/core/Base.h"
#include "VulkanError.h"
#include "VulkanLog.h"
#include "VulkanTimestampManager.h"

namespace quoll::rhi {

static constexpr u32 QueryCount = 1000;

VulkanTimestampManager::VulkanTimestampManager(VulkanDeviceObject &device)
    : mDevice(device) {

  VkQueryPoolCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.queryCount = QueryCount;
  createInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;

  for (usize i = 0; i < mQueryPools.size(); ++i) {
    VkQueryPool queryPool = VK_NULL_HANDLE;
    String name = "timestamp query pool frame " + std::to_string(i);
    checkForVulkanError(
        vkCreateQueryPool(mDevice, &createInfo, nullptr, &queryPool),
        "Cannot create query pool", name);

    mDevice.setObjectName(name, VK_OBJECT_TYPE_QUERY_POOL, queryPool);

    mQueryPools.at(i) = queryPool;
    LOG_DEBUG_VK("Query pool " << name << " created", queryPool);
  }
}

VulkanTimestampManager::~VulkanTimestampManager() {
  for (VkQueryPool queryPool : mQueryPools) {
    vkDestroyQueryPool(mDevice, queryPool, nullptr);
    LOG_DEBUG_VK("Query pool destroyed", queryPool);
  }
}

void VulkanTimestampManager::setCurrentFrame(u32 frameIndex) {
  mCurrentFrame = static_cast<usize>(frameIndex);
}

void VulkanTimestampManager::reset() {
  vkResetQueryPool(mDevice, mQueryPools.at(static_cast<usize>(mCurrentFrame)),
                   0, QueryCount);
}

} // namespace quoll::rhi
