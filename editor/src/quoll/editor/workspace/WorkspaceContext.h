#pragma once

#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/asset/AssetManager.h"

namespace quoll::editor {

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

} // namespace quoll::editor
