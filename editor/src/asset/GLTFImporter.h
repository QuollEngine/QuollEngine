#pragma once

#include "liquid/asset/Result.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/rhi/ResourceRegistry.h"

namespace liquidator {

class GLTFImporter {
public:
  GLTFImporter(liquid::AssetManager &assetManager,
               liquid::rhi::ResourceRegistry &deviceRegistry);

  liquid::Result<bool> loadFromFile(const liquid::String &filename,
                                    const std::filesystem::path &directory);

private:
  liquid::AssetManager &mAssetManager;
  liquid::rhi::ResourceRegistry &mDeviceRegistry;
};

} // namespace liquidator
