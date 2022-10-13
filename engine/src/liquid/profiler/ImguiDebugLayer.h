#pragma once

#include "liquid/profiler/FPSCounter.h"
#include "liquid/rhi/PhysicalDeviceInformation.h"
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
   * @param fpsCounter FPS counter
   */
  ImguiDebugLayer(const rhi::PhysicalDeviceInformation &physicalDeviceInfo,
                  const rhi::DeviceStats &deviceStats,
                  const FPSCounter &fpsCounter);

  /**
   * @brief Render debug menu
   */
  void renderMenu();

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
  void renderTableRow(StringView header, StringView value);

  /**
   * @brief Render property map as a table
   *
   * @param properties Properties
   */
  void renderPropertyMapAsTable(
      const std::vector<std::pair<String, Property>> &properties);

private:
  rhi::PhysicalDeviceInformation mPhysicalDeviceInfo;
  const FPSCounter &mFpsCounter;
  const rhi::DeviceStats &mDeviceStats;

  bool mUsageMetricsVisible = false;
  bool mPhysicalDeviceInfoVisible = false;
  bool mPerformanceMetricsVisible = false;
};

} // namespace liquid
