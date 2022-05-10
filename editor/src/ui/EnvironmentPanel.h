#pragma once

#include "liquid/asset/AssetManager.h"
#include "../editor-scene/SceneManager.h"

namespace liquidator {

/**
 * @brief Environment panel component
 */
class EnvironmentPanel {
public:
  /**
   * @brief Render environment panel
   *
   * @param sceneManager Scene manager
   * @param assetManager Asset manager
   */
  void render(SceneManager &sceneManager, liquid::AssetManager &assetManager);
};

} // namespace liquidator
