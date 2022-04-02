#pragma once

#include "liquid/asset/AssetRegistry.h"

namespace liquid {

class DebugAssetBrowser {
public:
  /**
   * @brief Create menu bar
   *
   * @param registry Asset registry
   */
  DebugAssetBrowser(liquid::AssetRegistry &registry);

  /**
   * @brief Render menu bar
   */
  void render();

private:
  liquid::AssetRegistry &mRegistry;

  uint32_t mSelectedType = 0;
  uint32_t mSelectedObject = 0;
  bool mOpen = false;
};

} // namespace liquid
