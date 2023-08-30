#pragma once

#include "quoll/editor/project/Project.h"

namespace quoll::editor {

/**
 * @brief Project manager
 *
 * Creates or loads projects
 */
class ProjectManager {
public:
  /**
   * @brief Create project in path
   *
   * @return Project created successfully
   */
  bool createProjectInPath();

  /**
   * @brief Open project in path
   *
   * @return Project opened successfully
   */
  bool openProjectInPath();

  /**
   * @brief Get project
   *
   * @return Project
   */
  inline Project getProject() const { return mProject; }

private:
  Project mProject;
};

} // namespace quoll::editor
