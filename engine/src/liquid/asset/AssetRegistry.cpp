#include "liquid/core/Base.h"
#include "AssetRegistry.h"

namespace liquid {

AssetRegistry::~AssetRegistry() {
  for (auto &[_, texture] : mTextures.getAssets()) {
    delete[](texture.data.data);
  }
}

void AssetRegistry::syncWithDeviceRegistry(rhi::ResourceRegistry &registry) {
  for (auto &[_, texture] : mTextures.getAssets()) {
    if (texture.data.deviceHandle == rhi::TextureHandle::Invalid) {
      rhi::TextureDescription description;

      description.data = texture.data.data;
      description.width = texture.data.width;
      description.layers = texture.data.layers;
      description.height = texture.data.height;
      description.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
      description.type = rhi::TextureType::Standard;
      description.size = texture.size;
      description.format = VK_FORMAT_R8G8B8A8_SRGB;

      texture.data.deviceHandle = registry.setTexture(description);
    }
  }
}

} // namespace liquid
