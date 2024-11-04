#pragma once

#include "quoll/profiler/DebugPanel.h"

namespace quoll::rhi {

class RenderDevice;

}

namespace quoll::debug {

class RendererDebugPanel : public DebugPanel {
public:
  RendererDebugPanel(rhi::RenderDevice *device);

  void onRenderMenu() override;

  void onRender() override;

private:
  void renderPhysicalDeviceInfo();

  void renderUsageMetrics();

private:
  rhi::RenderDevice *mDevice;

  bool mPhysicalDeviceInfoOpen = false;
  bool mUsageMetricsOpen = false;
};

} // namespace quoll::debug
