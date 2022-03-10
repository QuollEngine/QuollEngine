#include "liquid/core/Base.h"
#include "ResourceRegistry.h"

namespace liquid::experimental {

BufferHandle ResourceRegistry::addBuffer(const BufferDescription &description) {
  return mBuffers.addDescription(description);
}

void ResourceRegistry::deleteBuffer(BufferHandle handle) {
  mBuffers.deleteDescription(handle);
}

void ResourceRegistry::updateBuffer(BufferHandle handle,
                                    const BufferDescription &description) {
  mBuffers.updateDescription(handle, description);
}

TextureHandle
ResourceRegistry::addTexture(const TextureDescription &description) {
  return mTextures.addDescription(description);
}

void ResourceRegistry::deleteTexture(TextureHandle handle) {
  mTextures.deleteDescription(handle);
}

} // namespace liquid::experimental
