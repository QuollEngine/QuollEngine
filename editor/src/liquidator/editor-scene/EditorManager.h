#pragma once

#include "liquidator/project/Project.h"
#include "liquidator/state/WorkspaceState.h"

namespace liquid::editor {

/**
 * @brief Editor manager
 *
 * Saves an loads workspace state
 */
class EditorManager {
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
   * @param path Path to workspace state file
   * @param state Workspace state
   */
  static void loadWorkspaceState(const std::filesystem::path &path,
                                 WorkspaceState &state);
};

} // namespace liquid::editor
