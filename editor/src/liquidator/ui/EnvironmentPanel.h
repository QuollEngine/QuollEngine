#pragma once

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"

namespace liquid::editor {

/**
 * @brief Environment panel component
 */
class EnvironmentPanel {
public:
  /**
   * @brief Render environment panel
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  static void renderContent(WorkspaceState &state, AssetRegistry &assetRegistry,
                            ActionExecutor &actionExecutor);

private:
  /**
   * @brief Render environment skybox section
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  static void renderSkyboxSection(Scene &scene, AssetRegistry &assetRegistry,
                                  ActionExecutor &actionExecutor);

  /**
   * @brief Render environment lighting section
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  static void renderLightingSection(Scene &scene, AssetRegistry &assetRegistry,
                                    ActionExecutor &actionExecutor);
};

} // namespace liquid::editor
