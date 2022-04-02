#include "liquid/core/Base.h"
#include "AssetRegistry.h"

namespace liquid {

AssetRegistry::~AssetRegistry() {
  for (auto &[_, texture] : mTextures.getAssets()) {
    delete[](texture.data.data);
  }
}

} // namespace liquid
