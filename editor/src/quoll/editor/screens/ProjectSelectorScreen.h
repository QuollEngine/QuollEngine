#pragma once

#include "quoll/window/Window.h"
#include "quoll/rhi/RenderDevice.h"

#include "../project/ProjectManager.h"

namespace quoll::editor {

/**
 * @brief Project selector screen
 */
class ProjectSelectorScreen {
public:
  /**
   * @brief Create project selector screen
   *
   * @param window Window
   * @param device Render device
   */
  ProjectSelectorScreen(Window &window, rhi::RenderDevice *device);

  /**
   * @brief Start project selector screen
   *
   * @return Optional project
   */
  std::optional<Project> start();

private:
  Window &mWindow;
  rhi::RenderDevice *mDevice;
};

} // namespace quoll::editor
