#pragma once

#include "liquidator/asset/AssetManager.h"
#include "liquidator/editor-scene/EditorManager.h"

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
  static void render(EditorManager &editorManager, AssetManager &assetManager);
};

} // namespace liquidator
