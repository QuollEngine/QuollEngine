#pragma once

#include "quoll/rhi/RenderDevice.h"
#include "quoll/window/Window.h"
#include "../project/ProjectManager.h"

namespace quoll::editor {

class ProjectSelectorWindow {
public:
  ProjectSelectorWindow(Window &window, rhi::RenderDevice *device);

  std::optional<Project> start();

private:
  Window &mWindow;
  rhi::RenderDevice *mDevice;
};

} // namespace quoll::editor
