#pragma once

#include "PhysicalDeviceInformation.h"
#include "DebugAssetBrowser.h"
#include "liquid/profiler/FPSCounter.h"
#include "liquid/profiler/DebugManager.h"
#include "liquid/rhi/ResourceRegistry.h"
#include "liquid/rhi/DeviceStats.h"

namespace liquid {

class ImguiDebugLayer {
public:
  /**
   * @brief Set properties for render
   *
   * @param physicalDeviceInfo Physical device information
   * @param deviceStats Device stats
   * @param registry Resource registry
   * @param assetRegistry Asset registry
   * @param fpsCounter FPS counter
   * @param debugManager Debug manager
   */
  ImguiDebugLayer(const PhysicalDeviceInformation &physicalDeviceInfo,
                  const rhi::DeviceStats &deviceStats,
                  rhi::ResourceRegistry &registry, AssetRegistry &assetRegistry,
                  const FPSCounter &fpsCounter, DebugManager &debugManager);

  /**
   * @brief Render debug UI
   */
  void render();

private:
  /**
   * @brief Render performance metrics
   */
  void renderPerformanceMetrics();

  /**
   * @brief Render physical device information
   */
  void renderPhysicalDeviceInfo();

  /**
   * @brief Render usage metrics
   */
  void renderUsageMetrics();

  /**
   * @brief Render asset browser
   */
  void renderAssetBrowser();

  /**
   * @brief Render two col row
   *
   * @param header Row header
   * @param value Row value
   */
  void renderTableRow(const String &header, const String &value);

  /**
   * @brief Render property map as a table
   *
   * @param properties Properties
   */
  void renderPropertyMapAsTable(
      const std::vector<std::pair<String, Property>> &properties);

private:
  PhysicalDeviceInformation mPhysicalDeviceInfo;
  const FPSCounter &mFpsCounter;
  DebugManager &mDebugManager;
  const rhi::DeviceStats &mDeviceStats;
  rhi::ResourceRegistry &mResourceRegistry;
  DebugAssetBrowser mAssetBrowser;

  bool mUsageMetricsVisible = false;
  bool mPhysicalDeviceInfoVisible = false;
  bool mPerformanceMetricsVisible = false;
  bool mWireframeModeEnabled = false;
  bool mAssetBrowserVisible = false;
};

} // namespace liquid
