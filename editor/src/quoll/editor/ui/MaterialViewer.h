#pragma once

#include "quoll/asset/AssetRegistry.h"

namespace quoll::editor {

class MaterialViewer {
public:
  void render(AssetRegistry &assetRegistry);

  void open(AssetHandle<MaterialAsset> handle);

private:
  AssetHandle<MaterialAsset> mHandle;
};

} // namespace quoll::editor
