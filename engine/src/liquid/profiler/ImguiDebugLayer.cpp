#include "liquid/core/Base.h"
#include "liquid/core/Property.h"
#include "ImguiDebugLayer.h"
#include <imgui.h>

namespace liquid {

template <class T> String convertToString(T value) {
  return std::to_string(value);
}

ImguiDebugLayer::ImguiDebugLayer(
    const PhysicalDeviceInformation &physicalDeviceInfo,
    const rhi::DeviceStats &deviceStats, rhi::ResourceRegistry &registry,
    const FPSCounter &fpsCounter, DebugManager &debugManager)
    : mPhysicalDeviceInfo(physicalDeviceInfo), mResourceRegistry(registry),
      mFpsCounter(fpsCounter), mDeviceStats(deviceStats),
      mDebugManager(debugManager) {}

void ImguiDebugLayer::render() {
  if (ImGui::BeginMainMenuBar()) {

    if (ImGui::BeginMenu("Debug")) {
      ImGui::MenuItem("Physical Device Information", nullptr,
                      &mPhysicalDeviceInfoVisible);
      ImGui::MenuItem("Usage Metrics", nullptr, &mUsageMetricsVisible);

      ImGui::MenuItem("Performance Metrics", nullptr,
                      &mPerformanceMetricsVisible);

      if (ImGui::MenuItem("Wireframe Mode", nullptr, &mWireframeModeEnabled)) {
        mDebugManager.setWireframeMode(mWireframeModeEnabled);
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
  if (!mPerformanceMetricsVisible)
    return;

  uint32_t fps = mFpsCounter.getFPS();

  ImGui::Begin("Performance Metrics", &mPerformanceMetricsVisible,
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
  if (!mUsageMetricsVisible)
    return;

  ImGui::Begin("Usage Metrics", &mUsageMetricsVisible,
               ImGuiWindowFlags_NoDocking);

  if (ImGui::BeginTable("Table", 2,
                        ImGuiTableFlags_Borders |
                            ImGuiTableColumnFlags_WidthStretch |
                            ImGuiTableFlags_RowBg)) {

    size_t bufferSize = 0;
    for (auto &[_, description] :
         mResourceRegistry.getBufferMap().getDescriptions()) {
      bufferSize += description.size;
    }

    renderTableRow(
        "Number of buffers",
        convertToString(
            mResourceRegistry.getBufferMap().getDescriptions().size()));
    renderTableRow("Total size of allocated buffer",
                   convertToString(bufferSize));

    size_t textureSize = 0;
    for (auto &[_, description] :
         mResourceRegistry.getBufferMap().getDescriptions()) {
      textureSize += description.size;
    }

    renderTableRow(
        "Number of textures",
        convertToString(
            mResourceRegistry.getBufferMap().getDescriptions().size()));
    renderTableRow("Total size of allocated textures",
                   convertToString(textureSize));
    renderTableRow("Number of draw calls",
                   convertToString(mDeviceStats.getDrawCallsCount()));
    renderTableRow("Number of drawn primitives",
                   convertToString(mDeviceStats.getDrawnPrimitivesCount()));

    ImGui::EndTable();
  }

  ImGui::End();
}

void ImguiDebugLayer::renderPhysicalDeviceInfo() {
  if (!mPhysicalDeviceInfoVisible)
    return;

  ImGui::Begin(("Device Info: " + mPhysicalDeviceInfo.getName()).c_str(),
               &mPhysicalDeviceInfoVisible, ImGuiWindowFlags_NoDocking);
  if (ImGui::BeginTable("Table", 2,
                        ImGuiTableFlags_Borders |
                            ImGuiTableColumnFlags_WidthStretch)) {
    renderTableRow("Name", mPhysicalDeviceInfo.getName());
    renderTableRow("Type", mPhysicalDeviceInfo.getTypeString());

    ImGui::EndTable();
  }

  ImGui::Text("Properties");
  renderPropertyMapAsTable(mPhysicalDeviceInfo.getProperties());

  ImGui::Text("Limits");
  renderPropertyMapAsTable(mPhysicalDeviceInfo.getLimits());
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
