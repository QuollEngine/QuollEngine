#pragma once

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"
#include "liquidator/asset/AssetManager.h"

namespace liquid::editor {

/**
 * @brief Workspace context
 */
struct WorkspaceContext {
  /**
   * Workspace state
   */
  WorkspaceState &state;

  /**
   * Action executor
   */
  ActionExecutor &actionExecutor;

  /**
   * Asset manager
   */
  AssetManager &assetManager;
};

} // namespace liquid::editor