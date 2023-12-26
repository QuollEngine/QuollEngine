#pragma once

#include "WorkspaceManager.h"

namespace quoll::editor {

/**
 * @brief Worksapce tabs
 */
class WorkspaceTabs {
public:
  /**
   * @brief Render workspace tabs
   *
   * @param workspaceManager Workspace manager
   */
  static void render(WorkspaceManager &workspaceManager);
};

} // namespace quoll::editor
