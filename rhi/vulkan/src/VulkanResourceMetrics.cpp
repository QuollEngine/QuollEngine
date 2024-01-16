#include "quoll/core/Base.h"
#include "VulkanBuffer.h"
#include "VulkanResourceMetrics.h"
#include "VulkanTexture.h"

namespace quoll::rhi {

VulkanResourceMetrics::VulkanResourceMetrics(
    VulkanResourceRegistry &registry, VulkanDescriptorPool &descriptorPool)
    : mRegistry(registry), mDescriptorPool(descriptorPool) {}

usize VulkanResourceMetrics::getTotalBufferSize() const {
  usize size = 0;

  for (auto &[_, buffer] : mRegistry.getBuffers()) {
    size += buffer->getSize();
  }

  return size;
}

usize VulkanResourceMetrics::getBuffersCount() const {
  return mRegistry.getBuffers().size();
}

usize VulkanResourceMetrics::getTexturesCount() const {
  return mRegistry.getTextures().size();
}

usize VulkanResourceMetrics::getDescriptorsCount() const {
  return mDescriptorPool.getDescriptorsCount();
}

} // namespace quoll::rhi
