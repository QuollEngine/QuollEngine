#include "liquid/core/Base.h"
#include "VulkanResourceMetrics.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"

namespace liquid::rhi {

VulkanResourceMetrics::VulkanResourceMetrics(
    VulkanResourceRegistry &registry, VulkanDescriptorPool &descriptorPool)
    : mRegistry(registry), mDescriptorPool(descriptorPool) {}

size_t VulkanResourceMetrics::getTotalBufferSize() const {
  size_t size = 0;

  for (auto &[_, buffer] : mRegistry.getBuffers()) {
    size += buffer->getSize();
  }

  return size;
}

size_t VulkanResourceMetrics::getBuffersCount() const {
  return mRegistry.getBuffers().size();
}

size_t VulkanResourceMetrics::getTotalTextureSize() const {
  size_t size = 0;

  for (auto &[_, texture] : mRegistry.getTextures()) {
    size += texture->getDescription().size;
  }

  return size;
}

size_t VulkanResourceMetrics::getTexturesCount() const {
  return mRegistry.getTextures().size();
}

size_t VulkanResourceMetrics::getDescriptorsCount() const {
  return mDescriptorPool.getDescriptorsCount();
}

} // namespace liquid::rhi
