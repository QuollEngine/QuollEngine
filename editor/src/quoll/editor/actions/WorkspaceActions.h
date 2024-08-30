#pragma once

#include "quoll/editor/workspace/Workspace.h"
#include "quoll/editor/workspace/WorkspaceManager.h"
#include "Action.h"

namespace quoll::editor {

class AddWorkspace : public Action {
public:
  AddWorkspace(Workspace *workspace, WorkspaceManager &workspaceManager);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Workspace *mWorkspace;
  WorkspaceManager &mWorkspaceManager;
};

} // namespace quoll::editor
