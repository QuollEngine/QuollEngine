#pragma once

#include "quoll/profiler/FPSCounter.h"
#include "quoll/rhi/DeviceStats.h"
#include "quoll/rhi/PhysicalDeviceInformation.h"

namespace quoll {

class ImguiDebugLayer {
public:
  ImguiDebugLayer(const rhi::PhysicalDeviceInformation &physicalDeviceInfo,
                  const rhi::DeviceStats &deviceStats,
                  const FPSCounter &fpsCounter);

  void renderMenu();

  void render();

private:
  void renderPerformanceMetrics();

  void renderPhysicalDeviceInfo();

  void renderUsageMetrics();

  void renderDemoWindow();

  void renderTableRow(StringView header, StringView value);

private:
  rhi::PhysicalDeviceInformation mPhysicalDeviceInfo;
  const FPSCounter &mFpsCounter;
  const rhi::DeviceStats &mDeviceStats;

  bool mUsageMetricsVisible = false;
  bool mPhysicalDeviceInfoVisible = false;
  bool mPerformanceMetricsVisible = false;
  bool mDemoWindowVisible = false;
};

} // namespace quoll
