#pragma once

#include "PhysicalDeviceInformation.h"
#include "liquid/profiler/StatsManager.h"
#include "liquid/profiler/DebugManager.h"

namespace liquid {

class ImguiDebugLayer {
public:
  /**
   * @brief Set properties for render
   *
   * @param physicalDeviceInfo Physical device information
   * @param statsManager Stats manager
   * @param debugManager Debug manager
   */
  ImguiDebugLayer(const PhysicalDeviceInformation &physicalDeviceInfo,
                  const StatsManager &statsManager,
                  const SharedPtr<DebugManager> &debugManager_);

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
  PhysicalDeviceInformation physicalDeviceInfo;
  const StatsManager &statsManager;
  SharedPtr<DebugManager> debugManager = nullptr;

  bool usageMetricsVisible = false;
  bool physicalDeviceInfoVisible = false;
  bool performanceMetricsVisible = false;
  bool wireframeModeEnabled = false;
};

} // namespace liquid
