#pragma once

#include "quoll/editor/project/Project.h"
#include "quoll/editor/state/WorkspaceState.h"

namespace quoll::editor {

/**
 * @brief Workspace input/output
 *
 * Saves an loads workspace state
 */
class WorkspaceIO {
public:
  /**
   * @brief Save workspace state to a file
   *
   * @param state Workspace state
   * @param path Path to workspace state file
   */
  static void saveWorkspaceState(WorkspaceState &state,
                                 const std::filesystem::path &path);

  /**
   * @brief Load workspace state from file
   *
   * @param state Workspace state
   * @param path Path to workspace state file
   */
  static void loadWorkspaceState(WorkspaceState &state,
                                 const std::filesystem::path &path);
};

} // namespace quoll::editor
