#pragma once

#include "Workspace.h"

namespace quoll::editor {

/**
 * @brief Workspace manager
 *
 * Manages workspaces in editor
 */
class WorkspaceManager {
public:
  /**
   * @brief Add workspace
   *
   * @param workspace Workspace
   */
  void add(Workspace *workspace);

  /**
   * @brief Get current workspace
   *
   * @return Current workspace
   */
  Workspace *getCurrentWorkspace() {
    return mWorkspaces.at(mCurrentIndex).get();
  }

  /**
   * @brief Get all workspaces
   *
   * @return Workspaces
   */
  inline const std::vector<std::unique_ptr<Workspace>> &getWorkspaces() const {
    return mWorkspaces;
  };

private:
  std::vector<std::unique_ptr<Workspace>> mWorkspaces;
  size_t mCurrentIndex = 0;
};

} // namespace quoll::editor
