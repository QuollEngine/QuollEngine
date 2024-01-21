#pragma once

#include "quoll/rhi/RenderDevice.h"
#include "VulkanDeviceObject.h"

namespace quoll::rhi {

class VulkanTimestampManager {
public:
  VulkanTimestampManager(VulkanDeviceObject &device);

  ~VulkanTimestampManager();

  VulkanTimestampManager(const VulkanTimestampManager &) = delete;
  VulkanTimestampManager &operator=(const VulkanTimestampManager &) = delete;
  VulkanTimestampManager(VulkanTimestampManager &&) = delete;
  VulkanTimestampManager &operator=(VulkanTimestampManager &&) = delete;

  inline VkQueryPool getQueryPool() const {
    return mQueryPools.at(mCurrentFrame);
  }

  void reset();

  void setCurrentFrame(u32 frameIndex);

private:
  VulkanDeviceObject &mDevice;
  usize mCurrentFrame = 0;
  std::array<VkQueryPool, RenderDevice::NumFrames> mQueryPools{};
};

} // namespace quoll::rhi
