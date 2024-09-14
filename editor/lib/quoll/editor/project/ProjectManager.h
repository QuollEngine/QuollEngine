#pragma once

#include "quoll/editor/project/Project.h"

namespace quoll::editor {

class ProjectManager {
public:
  bool createProjectInPath();

  bool openProjectInPath();

  inline Project getProject() const { return mProject; }

private:
  Project mProject;
};

} // namespace quoll::editor
