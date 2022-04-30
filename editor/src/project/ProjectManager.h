#pragma once

namespace liquidator {

struct Project {
  liquid::String name;
  liquid::String version;
  std::filesystem::path assetsPath;
  std::filesystem::path scenePath;
  std::filesystem::path settingsPath;
};

class ProjectManager {
public:
  /**
   * Create project in path
   *
   * @return Project created successfully
   */
  bool createProjectInPath();

  /**
   * Open project in path
   *
   * @return Project opened successfully
   */
  bool openProjectInPath();

  /**
   * Get project
   *
   * @return Project
   */
  inline Project getProject() const { return mProject; }

private:
  Project mProject;
};

} // namespace liquidator
