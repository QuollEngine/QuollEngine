#include "quoll/core/Base.h"
#include "WorkspaceActions.h"

namespace quoll::editor {

AddWorkspace::AddWorkspace(Workspace *workspace,
                           WorkspaceManager &workspaceManager)
    : mWorkspace(workspace), mWorkspaceManager(workspaceManager) {}

ActionExecutorResult AddWorkspace::onExecute(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  mWorkspaceManager.add(mWorkspace);

  return {};
}

bool AddWorkspace::predicate(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  return true;
}

} // namespace quoll::editor
