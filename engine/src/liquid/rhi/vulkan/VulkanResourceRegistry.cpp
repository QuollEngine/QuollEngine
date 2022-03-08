#include "liquid/core/Base.h"
#include "VulkanResourceRegistry.h"

namespace liquid::experimental {

void VulkanResourceRegistry::addBuffer(BufferHandle handle,
                                       std::unique_ptr<VulkanBuffer> &&buffer) {
  mBuffers.insert({handle, std::move(buffer)});
}

void VulkanResourceRegistry::removeBuffer(BufferHandle handle) {
  mBuffers.erase(handle);
}

void VulkanResourceRegistry::updateBuffer(
    BufferHandle handle, std::unique_ptr<VulkanBuffer> &&buffer) {
  mBuffers.at(handle) = std::move(buffer);
}

void VulkanResourceRegistry::addTexture(
    TextureHandle handle, std::unique_ptr<VulkanTexture> &&texture) {
  mTextures.insert({handle, std::move(texture)});
}

void VulkanResourceRegistry::removeTexture(TextureHandle handle) {
  mTextures.erase(handle);
}

} // namespace liquid::experimental