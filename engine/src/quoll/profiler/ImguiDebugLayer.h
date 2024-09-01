#pragma once

#include "quoll/rhi/DeviceStats.h"
#include "quoll/rhi/PhysicalDeviceInformation.h"
#include "FPSCounter.h"
#include "MetricsCollector.h"

namespace quoll {

class AssetCache;

class ImguiDebugLayer {
public:
  ImguiDebugLayer(const rhi::PhysicalDeviceInformation &physicalDeviceInfo,
                  const rhi::DeviceStats &deviceStats,
                  const FPSCounter &fpsCounter,
                  MetricsCollector &metricsCollector, AssetCache *assetCache);

  void renderMenu();

  void render();

private:
  void renderPerformanceMetrics();

  void renderPhysicalDeviceInfo();

  void renderUsageMetrics();

  void renderAssets();

  void renderDemoWindow();

  void renderTableRow(StringView header, StringView value);

private:
  rhi::PhysicalDeviceInformation mPhysicalDeviceInfo;
  const FPSCounter &mFpsCounter;
  const rhi::DeviceStats &mDeviceStats;
  MetricsCollector &mMetricsCollector;
  AssetCache *mAssetCache;

  bool mUsageMetricsVisible = false;
  bool mPhysicalDeviceInfoVisible = false;
  bool mPerformanceMetricsVisible = false;
  bool mAssetsVisible = false;
  bool mDemoWindowVisible = false;
};

} // namespace quoll
