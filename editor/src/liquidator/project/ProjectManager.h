#pragma once

namespace liquidator {

/**
 * @brief Project data
 */
struct Project {
  /**
   * Project name
   */
  liquid::String name;

  /**
   * Project version
   */
  liquid::String version;

  /**
   * Path to assets directory
   */
  std::filesystem::path assetsPath;

  /**
   * Path to scene directory
   */
  std::filesystem::path scenePath;

  /**
   * Path to settings directory
   */
  std::filesystem::path settingsPath;
};

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

} // namespace liquidator
