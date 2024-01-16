#pragma once

#include "quoll/asset/AssetRegistry.h"

namespace quoll::editor {

class MaterialViewer {
public:
  void render(AssetRegistry &assetRegistry);

  void open(MaterialAssetHandle handle);

private:
  MaterialAssetHandle mHandle = MaterialAssetHandle::Null;
};

} // namespace quoll::editor
