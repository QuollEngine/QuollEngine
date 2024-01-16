#pragma once

#include "quoll/window/Window.h"
#include "quoll/rhi/RenderDevice.h"

#include "../project/ProjectManager.h"

namespace quoll::editor {

class EditorScreen {
public:
  EditorScreen(Window &window, InputDeviceManager &deviceManager,
               rhi::RenderDevice *device);

  void start(const Project &project);

private:
  InputDeviceManager &mDeviceManager;
  Window &mWindow;
  rhi::RenderDevice *mDevice;
};

} // namespace quoll::editor
