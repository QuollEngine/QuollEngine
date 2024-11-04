#pragma once

#include "quoll/profiler/DebugPanel.h"

namespace quoll::rhi {

class RenderDevice;

}

namespace quoll {

class FPSCounter;
class MetricsCollector;

} // namespace quoll

namespace quoll::debug {

class PerformanceDebugPanel : public DebugPanel {
public:
  PerformanceDebugPanel(rhi::RenderDevice *device,
                        MetricsCollector &metricsCollector,
                        const FPSCounter &fpsCounter);

  void onRenderMenu() override;

  void onRender() override;

private:
  rhi::RenderDevice *mDevice;
  MetricsCollector *mMetricsCollector;
  const FPSCounter *mFpsCounter;

  bool mOpen = false;
};

} // namespace quoll::debug
