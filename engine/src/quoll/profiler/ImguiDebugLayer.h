#pragma once

#include "quoll/profiler/FPSCounter.h"
#include "quoll/rhi/PhysicalDeviceInformation.h"
#include "quoll/rhi/DeviceStats.h"

namespace quoll {

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
   * @brief Render imgui demo window
   */
  void renderDemoWindow();

  /**
   * @brief Render two col row
   *
   * @param header Row header
   * @param value Row value
   */
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
