#include "quoll/core/Base.h"
#include "WorkspaceManager.h"

namespace quoll::editor {

void WorkspaceManager::add(Workspace *workspace) {
  size_t found = mWorkspaces.size();
  for (size_t i = 0; i < mWorkspaces.size(); ++i) {
    auto &existing = mWorkspaces.at(i);
    const auto &m1 = existing->getMatchParams();
    const auto &m2 = workspace->getMatchParams();

    if (m1.type == m2.type && m1.asset == m2.asset &&
        m1.assetType == m2.assetType) {
      found = i;
      break;
    }
  }

  if (found < mWorkspaces.size()) {
    mWorkspaces.at(found).reset(workspace);
  } else {
    mWorkspaces.push_back(std::unique_ptr<Workspace>(workspace));
  }
}

} // namespace quoll::editor
