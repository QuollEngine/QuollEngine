#pragma once

#include "quoll/core/Result.h"

namespace quoll::editor {

class AssetManager;

class AssetLoader {
public:
  AssetLoader(AssetManager &assetManager);

  Result<Path> loadFromPath(const Path &path, const Path &directory);

  Result<void> loadFromFileDialog(const Path &directory);

private:
  AssetManager &mAssetManager;
};

} // namespace quoll::editor
