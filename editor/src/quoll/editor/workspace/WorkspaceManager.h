#pragma once

#include "Workspace.h"

namespace quoll::editor {

class WorkspaceManager {
public:
  void add(Workspace *workspace);

  Workspace *getCurrentWorkspace() {
    return mWorkspaces.at(mCurrentIndex).get();
  }

  inline size_t getCurrentWorkspaceIndex() const { return mCurrentIndex; }

  inline const std::vector<std::unique_ptr<Workspace>> &getWorkspaces() const {
    return mWorkspaces;
  };

  void switchWorkspace(size_t index);

private:
  std::vector<std::unique_ptr<Workspace>> mWorkspaces;
  size_t mCurrentIndex = 0;
};

} // namespace quoll::editor
