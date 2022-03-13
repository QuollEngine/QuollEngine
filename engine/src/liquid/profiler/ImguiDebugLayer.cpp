#include "liquid/core/Base.h"
#include "liquid/core/Property.h"
#include "ImguiDebugLayer.h"
#include <imgui.h>

namespace liquid {

template <class T> String convertToString(T value) {
  return std::to_string(value);
}

ImguiDebugLayer::ImguiDebugLayer(
    const PhysicalDeviceInformation &physicalDeviceInfo_,
    const StatsManager &statsManager_, DebugManager &debugManager_)
    : physicalDeviceInfo(physicalDeviceInfo_), statsManager(statsManager_),
      debugManager(debugManager_) {}

void ImguiDebugLayer::render() {
  if (ImGui::BeginMainMenuBar()) {

    if (ImGui::BeginMenu("Debug")) {
      ImGui::MenuItem("Physical Device Information", nullptr,
                      &physicalDeviceInfoVisible);
      ImGui::MenuItem("Usage Metrics", nullptr, &usageMetricsVisible);

      ImGui::MenuItem("Performance Metrics", nullptr,
                      &performanceMetricsVisible);

      if (ImGui::MenuItem("Wireframe Mode", nullptr, &wireframeModeEnabled)) {
        debugManager.setWireframeMode(wireframeModeEnabled);
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  renderPhysicalDeviceInfo();
  renderUsageMetrics();
  renderPerformanceMetrics();
}

void ImguiDebugLayer::renderPerformanceMetrics() {
  const uint32_t ONE_SECOND_IN_MS = 1000;
  if (!performanceMetricsVisible)
    return;

  uint32_t fps = statsManager.getFPS();

  ImGui::Begin("Performance Metrics", &performanceMetricsVisible,
               ImGuiWindowFlags_NoDocking);
  if (ImGui::BeginTable("Table", 2,
                        ImGuiTableFlags_Borders |
                            ImGuiTableColumnFlags_WidthStretch |
                            ImGuiTableFlags_RowBg)) {

    renderTableRow("FPS", std::to_string(fps));
    renderTableRow("Frame time",
                   std::to_string(fps > 0 ? ONE_SECOND_IN_MS / fps : 0) + "ms");
    ImGui::EndTable();
  }
  ImGui::End();
}

void ImguiDebugLayer::renderUsageMetrics() {
  if (!usageMetricsVisible)
    return;

  ImGui::Begin("Usage Metrics", &usageMetricsVisible,
               ImGuiWindowFlags_NoDocking);

  if (ImGui::BeginTable("Table", 2,
                        ImGuiTableFlags_Borders |
                            ImGuiTableColumnFlags_WidthStretch |
                            ImGuiTableFlags_RowBg)) {

    renderTableRow("Number of buffers",
                   convertToString(statsManager.getAllocatedBuffersCount()));
    renderTableRow("Total size of allocated buffer",
                   convertToString(statsManager.getAllocatedBuffersSize()));
    renderTableRow("Number of textures",
                   convertToString(statsManager.getAllocatedTexturesCount()));
    renderTableRow("Total size of allocated textures",
                   convertToString(statsManager.getAllocatedTexturesSize()));
    renderTableRow("Number of draw calls",
                   convertToString(statsManager.getDrawCallsCount()));
    renderTableRow("Number of drawn primitives",
                   convertToString(statsManager.getDrawnPrimitivesCount()));

    ImGui::EndTable();
  }

  ImGui::End();
}

void ImguiDebugLayer::renderPhysicalDeviceInfo() {
  if (!physicalDeviceInfoVisible)
    return;

  ImGui::Begin(("Device Info: " + physicalDeviceInfo.getName()).c_str(),
               &physicalDeviceInfoVisible, ImGuiWindowFlags_NoDocking);
  if (ImGui::BeginTable("Table", 2,
                        ImGuiTableFlags_Borders |
                            ImGuiTableColumnFlags_WidthStretch)) {
    renderTableRow("Name", physicalDeviceInfo.getName());
    renderTableRow("Type", physicalDeviceInfo.getTypeString());

    ImGui::EndTable();
  }

  ImGui::Text("Properties");
  renderPropertyMapAsTable(physicalDeviceInfo.getProperties());

  ImGui::Text("Limits");
  renderPropertyMapAsTable(physicalDeviceInfo.getLimits());
  ImGui::End();
}

void ImguiDebugLayer::renderPropertyMapAsTable(
    const std::vector<std::pair<String, Property>> &properties) {
  if (ImGui::BeginTable("Table", 2,
                        ImGuiTableFlags_Borders |
                            ImGuiTableColumnFlags_WidthStretch |
                            ImGuiTableFlags_RowBg)) {

    for (const auto &[name, value] : properties) {
      renderTableRow(name, value.toString());
    }
    ImGui::EndTable();
  }
}

void ImguiDebugLayer::renderTableRow(const String &header,
                                     const String &value) {
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%s", header.c_str());
  ImGui::TableSetColumnIndex(1);
  ImGui::Text("%s", value.c_str());
}

} // namespace liquid
