#pragma once

#include "liquid/asset/AssetManager.h"
#include "liquid/rhi/ResourceRegistry.h"

namespace liquidator {

class GLTFImporter {
public:
  GLTFImporter(liquid::AssetManager &assetManager,
               liquid::rhi::ResourceRegistry &deviceRegistry);

  void loadFromFile(const liquid::String &filename);

private:
  liquid::AssetManager &mAssetManager;
  liquid::rhi::ResourceRegistry &mDeviceRegistry;
};

} // namespace liquidator
