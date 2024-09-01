#pragma once

#include "quoll/asset/AssetRef.h"
#include "quoll/renderer/MaterialAsset.h"

namespace quoll::editor {

class MaterialViewer {
public:
  void render();

  void open(AssetRef<MaterialAsset> material);

private:
  AssetRef<MaterialAsset> mMaterial;
};

} // namespace quoll::editor
