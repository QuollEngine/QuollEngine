#include "quoll/core/Base.h"
#include "WorkspaceActions.h"

namespace quoll::editor {

AddWorkspace::AddWorkspace(Workspace *workspace,
                           WorkspaceManager &workspaceManager)
    : mWorkspace(workspace), mWorkspaceManager(workspaceManager) {}

ActionExecutorResult AddWorkspace::onExecute(WorkspaceState &state,
                                             AssetCache &assetCache) {
  mWorkspaceManager.add(mWorkspace);

  return {};
}

bool AddWorkspace::predicate(WorkspaceState &state,
                             AssetCache &assetCache) {
  return true;
}

} // namespace quoll::editor
