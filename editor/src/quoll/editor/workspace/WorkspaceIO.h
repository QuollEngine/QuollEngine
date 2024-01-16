#pragma once

#include "quoll/editor/project/Project.h"
#include "quoll/editor/workspace/WorkspaceState.h"

namespace quoll::editor {

class WorkspaceIO {
public:
  static void saveWorkspaceState(WorkspaceState &state,
                                 const std::filesystem::path &path);

  static void loadWorkspaceState(WorkspaceState &state,
                                 const std::filesystem::path &path);
};

} // namespace quoll::editor
