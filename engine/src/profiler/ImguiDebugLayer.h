#pragma once

#include "PhysicalDeviceInformation.h"
#include "renderer/vulkan/VulkanContext.h"
#include "profiler/StatsManager.h"
#include "profiler/DebugManager.h"

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
                  const SharedPtr<StatsManager> &statsManager,
                  const SharedPtr<DebugManager> &debugManager_);

  /**
   * @brief Collect frames per second
   *
   * @param fps Frames per second
   */
  void collectFPS(uint32_t fps);

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
  SharedPtr<StatsManager> statsManager = nullptr;
  SharedPtr<DebugManager> debugManager = nullptr;
  uint32_t fps = 0;

  bool usageMetricsVisible = false;
  bool physicalDeviceInfoVisible = false;
  bool performanceMetricsVisible = false;
  bool wireframeModeEnabled = false;
};

} // namespace liquid
