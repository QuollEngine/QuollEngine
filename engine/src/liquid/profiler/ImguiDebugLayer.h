#pragma once

#include "PhysicalDeviceInformation.h"
#include "liquid/profiler/FPSCounter.h"
#include "liquid/profiler/DebugManager.h"
#include "liquid/rhi/ResourceRegistry.h"
#include "liquid/rhi/DeviceStats.h"

namespace liquid {

/**
 * @brief Imgui debug layer
 *
 * Provides performance and debug
 * information for the engine
 */
class ImguiDebugLayer {
public:
  /**
   * @brief Set properties for render
   *
   * @param physicalDeviceInfo Physical device information
   * @param deviceStats Device stats
   * @param registry Resource registry
   * @param fpsCounter FPS counter
   * @param debugManager Debug manager
   */
  ImguiDebugLayer(const PhysicalDeviceInformation &physicalDeviceInfo,
                  const rhi::DeviceStats &deviceStats,
                  rhi::ResourceRegistry &registry, const FPSCounter &fpsCounter,
                  DebugManager &debugManager);

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

  bool mUsageMetricsVisible = false;
  bool mPhysicalDeviceInfoVisible = false;
  bool mPerformanceMetricsVisible = false;
  bool mWireframeModeEnabled = false;
};

} // namespace liquid
