#pragma once

#include "quoll/rhi/NativeResourceMetrics.h"
#include "VulkanResourceRegistry.h"
#include "VulkanDescriptorPool.h"

namespace quoll::rhi {

class VulkanResourceMetrics : public NativeResourceMetrics {
public:
  VulkanResourceMetrics(VulkanResourceRegistry &registry,
                        VulkanDescriptorPool &descriptorPool);

  usize getTotalBufferSize() const override;

  usize getBuffersCount() const override;

  usize getTexturesCount() const override;

  usize getDescriptorsCount() const override;

private:
  VulkanResourceRegistry &mRegistry;
  VulkanDescriptorPool &mDescriptorPool;
};

} // namespace quoll::rhi
