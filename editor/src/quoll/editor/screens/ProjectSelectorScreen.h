#pragma once

#include "quoll/window/Window.h"
#include "quoll/rhi/RenderDevice.h"

#include "../project/ProjectManager.h"

namespace quoll::editor {

class ProjectSelectorScreen {
public:
  ProjectSelectorScreen(Window &window, rhi::RenderDevice *device);

  std::optional<Project> start();

private:
  Window &mWindow;
  rhi::RenderDevice *mDevice;
};

} // namespace quoll::editor
