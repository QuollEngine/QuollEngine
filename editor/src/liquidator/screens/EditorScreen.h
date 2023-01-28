#pragma once

#include "liquid/events/EventSystem.h"
#include "liquid/window/Window.h"
#include "liquid/rhi/RenderDevice.h"

#include "../project/ProjectManager.h"

namespace liquid::editor {

/**
 * @brief Editor screen
 *
 * Main screen that shows the entire
 * editor
 */
class EditorScreen {
public:
  /**
   * @brief Create editor screen
   *
   * @param window Window
   * @param eventSystem Event system
   * @param device Render device
   */
  EditorScreen(Window &window, EventSystem &eventSystem,
               rhi::RenderDevice *device);

  /**
   * @brief Start editor screen
   *
   * @param project Project
   */
  void start(const Project &project);

private:
  Window &mWindow;
  EventSystem &mEventSystem;
  rhi::RenderDevice *mDevice;
};

} // namespace liquid::editor
