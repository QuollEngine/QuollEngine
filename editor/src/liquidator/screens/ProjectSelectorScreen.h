#pragma once

#include "quoll/events/EventSystem.h"
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
   * @param eventSystem Event system
   * @param device Render device
   */
  ProjectSelectorScreen(Window &window, EventSystem &eventSystem,
                        rhi::RenderDevice *device);

  /**
   * @brief Start project selector screen
   *
   * @return Optional project
   */
  std::optional<Project> start();

private:
  Window &mWindow;
  EventSystem &mEventSystem;
  rhi::RenderDevice *mDevice;
};

} // namespace quoll::editor
