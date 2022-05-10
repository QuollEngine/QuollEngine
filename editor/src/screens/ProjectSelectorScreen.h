#pragma once

#include "liquid/events/EventSystem.h"
#include "liquid/window/Window.h"
#include "liquid/rhi/RenderDevice.h"

#include "../project/ProjectManager.h"

namespace liquidator {

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
  ProjectSelectorScreen(liquid::Window &window,
                        liquid::EventSystem &eventSystem,
                        liquid::rhi::RenderDevice *device);

  /**
   * @brief Start project selector screen
   *
   * @return Optional project
   */
  std::optional<Project> start();

private:
  liquid::Window &mWindow;
  liquid::EventSystem &mEventSystem;
  liquid::rhi::RenderDevice *mDevice;
};

} // namespace liquidator
