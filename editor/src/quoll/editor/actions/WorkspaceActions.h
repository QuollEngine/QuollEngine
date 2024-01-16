#pragma once

#include "Action.h"
#include "quoll/editor/workspace/Workspace.h"
#include "quoll/editor/workspace/WorkspaceManager.h"

namespace quoll::editor {

class AddWorkspace : public Action {
public:
  AddWorkspace(Workspace *workspace, WorkspaceManager &workspaceManager);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Workspace *mWorkspace;
  WorkspaceManager &mWorkspaceManager;
};

} // namespace quoll::editor
