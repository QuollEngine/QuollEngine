#pragma once

#include "liquid/asset/AssetRegistry.h"

namespace liquid {

class DebugAssetBrowser {
public:
  /**
   * @brief Create asset browser
   *
   * @param registry Asset registry
   */
  DebugAssetBrowser(liquid::AssetRegistry &registry);

  /**
   * @brief Render asset browser
   */
  void render();

  /**
   * @brief Set on load to scene button handler
   *
   * @param handler On load to scene handler
   */
  void setOnLoadToScene(std::function<void(AssetType, uint32_t)> &&handler);

private:
  liquid::AssetRegistry &mRegistry;

  AssetType mSelectedType = AssetType::None;
  uint32_t mSelectedObject = 0;
  bool mOpen = false;

  std::function<void(AssetType, uint32_t)> mOnLoadToScene;
};

} // namespace liquid
