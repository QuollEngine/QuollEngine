#pragma once

#include "liquid/asset/AssetRegistry.h"

namespace liquidator {

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
  void render(liquid::AssetRegistry &assetRegistry);

  /**
   * @brief Open material viewer
   *
   * @param handle Material handle to display
   */
  void open(liquid::MaterialAssetHandle handle);

private:
  liquid::MaterialAssetHandle mHandle = liquid::MaterialAssetHandle::Invalid;
};

} // namespace liquidator
