#pragma once

#include "quoll/rhi/RenderDevice.h"
#include "quoll/window/Window.h"
#include "../project/ProjectManager.h"

namespace quoll::editor {

class EditorWindow {
public:
  EditorWindow(Window &window, InputDeviceManager &deviceManager,
               rhi::RenderDevice *device);

  void start(const Project &project);

private:
  InputDeviceManager &mDeviceManager;
  Window &mWindow;
  rhi::RenderDevice *mDevice;
};

} // namespace quoll::editor
