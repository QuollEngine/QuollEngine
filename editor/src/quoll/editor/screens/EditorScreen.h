#pragma once

#include "quoll/events/EventSystem.h"
#include "quoll/window/Window.h"
#include "quoll/rhi/RenderDevice.h"

#include "../project/ProjectManager.h"

namespace quoll::editor {

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
   * @param deviceManager Device manager
   * @param eventSystem Event system
   * @param device Render device
   */
  EditorScreen(Window &window, InputDeviceManager &deviceManager,
               EventSystem &eventSystem, rhi::RenderDevice *device);

  /**
   * @brief Start editor screen
   *
   * @param project Project
   */
  void start(const Project &project);

private:
  InputDeviceManager &mDeviceManager;
  Window &mWindow;
  EventSystem &mEventSystem;
  rhi::RenderDevice *mDevice;
};

} // namespace quoll::editor
