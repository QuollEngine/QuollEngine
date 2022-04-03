#pragma once

#include "liquid/asset/AssetRegistry.h"
#include "liquid/rhi/ResourceRegistry.h"

namespace liquidator {

class GLTFImporter {
public:
  GLTFImporter(liquid::AssetRegistry &registry,
               liquid::rhi::ResourceRegistry &deviceRegistry);

  void loadFromFile(const liquid::String &filename);

private:
  liquid::AssetRegistry &mAssetRegistry;
  liquid::rhi::ResourceRegistry &mDeviceRegistry;
};

} // namespace liquidator
