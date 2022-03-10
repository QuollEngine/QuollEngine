#pragma once

#include "../RenderHandle.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"

namespace liquid::experimental {

class VulkanResourceRegistry {
  template <class THandle, class TVulkanObject>
  using VulkanResourceMap =
      std::unordered_map<THandle, std::unique_ptr<TVulkanObject>>;

  using BufferMap = VulkanResourceMap<BufferHandle, VulkanBuffer>;
  using TextureMap = VulkanResourceMap<TextureHandle, VulkanTexture>;

public:
  void addBuffer(BufferHandle handle, std::unique_ptr<VulkanBuffer> &&buffer);

  void removeBuffer(BufferHandle handle);

  void updateBuffer(BufferHandle handle,
                    std::unique_ptr<VulkanBuffer> &&buffer);

  inline const std::unique_ptr<VulkanBuffer> &
  getBuffer(BufferHandle handle) const {
    return mBuffers.at(handle);
  }

  inline const BufferMap &getBuffers() const { return mBuffers; }

  void addTexture(TextureHandle handle,
                  std::unique_ptr<VulkanTexture> &&texture);

  void removeTexture(TextureHandle handle);

  inline const std::unique_ptr<VulkanTexture> &
  getTexture(TextureHandle handle) const {
    return mTextures.at(handle);
  }

  inline const TextureMap &getTextures() const { return mTextures; }

private:
  BufferMap mBuffers;
  TextureMap mTextures;
};

} // namespace liquid::experimental
