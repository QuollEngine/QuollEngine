#pragma once

#include "quoll/asset/AssetRegistry.h"

namespace quoll::editor {

/**
 * @brief Material viewer
 */
class MaterialViewer {
public:
  /**
   * @brief Render material viewer
   *
   * @param assetRegistry Asset registry
   */
  void render(AssetRegistry &assetRegistry);

  /**
   * @brief Open material viewer
   *
   * @param handle Material handle to display
   */
  void open(MaterialAssetHandle handle);

private:
  MaterialAssetHandle mHandle = MaterialAssetHandle::Null;
};

} // namespace quoll::editor
