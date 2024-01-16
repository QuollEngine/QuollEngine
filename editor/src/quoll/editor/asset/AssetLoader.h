#pragma once

#include "AssetManager.h"

namespace quoll::editor {

class AssetLoader {
public:
  AssetLoader(AssetManager &assetManager);

  Result<Path> loadFromPath(const Path &path, const Path &directory);

  Result<bool> loadFromFileDialog(const Path &directory);

private:
  AssetManager &mAssetManager;
};

} // namespace quoll::editor
