#pragma once

#include "VulkanHeaders.h"

namespace quoll::rhi {

class VulkanQueueFamily {
public:
  VulkanQueueFamily() = default;

  VulkanQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface);

  inline bool isComplete() const {
    return mGraphicsFamily.has_value() && mPresentFamily.has_value() &&
           mTransferFamily.has_value();
  }

  inline const std::array<u32, 3> toArray() const {
    return {getGraphicsFamily(), getTransferFamily(), getPresentFamily()};
  }

  inline u32 getGraphicsFamily() const { return mGraphicsFamily.value(); }

  inline u32 getPresentFamily() const { return mPresentFamily.value(); }

  inline u32 getTransferFamily() const { return mTransferFamily.value(); }

private:
  std::optional<u32> mGraphicsFamily;
  std::optional<u32> mPresentFamily;
  std::optional<u32> mTransferFamily;
};

} // namespace quoll::rhi
