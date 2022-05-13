#pragma once

#include "liquid/asset/AssetManager.h"
#include "../editor-scene/EditorManager.h"

namespace liquidator {

/**
 * @brief Environment panel component
 */
class EnvironmentPanel {
public:
  /**
   * @brief Render environment panel
   *
   * @param editorManager Editor manager
   * @param assetManager Asset manager
   */
  void render(EditorManager &editorManager, liquid::AssetManager &assetManager);
};

} // namespace liquidator
