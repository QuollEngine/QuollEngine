#pragma once

#include "liquid/asset/AssetRegistry.h"

namespace liquid::editor {

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
  MaterialAssetHandle mHandle = MaterialAssetHandle::Invalid;
};

} // namespace liquid::editor
