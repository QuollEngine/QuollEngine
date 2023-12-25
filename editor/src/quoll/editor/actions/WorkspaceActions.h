#pragma once

#include "Action.h"
#include "quoll/editor/workspace/Workspace.h"
#include "quoll/editor/workspace/WorkspaceManager.h"

namespace quoll::editor {

/**
 * @brief Start simulation mode action
 */
class AddWorkspace : public Action {
public:
  /**
   * @brief Create action
   *
   * @param workspace Workspace
   * @param workspaceManager Workspace manager
   */
  AddWorkspace(Workspace *workspace, WorkspaceManager &workspaceManager);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Workspace *mWorkspace;
  WorkspaceManager &mWorkspaceManager;
};

} // namespace quoll::editor
