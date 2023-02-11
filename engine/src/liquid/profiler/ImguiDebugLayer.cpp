#include "liquid/core/Base.h"
#include "liquid/core/Property.h"
#include "ImguiDebugLayer.h"
#include "liquid/imgui/Imgui.h"

namespace liquid {

ImguiDebugLayer::ImguiDebugLayer(
    const rhi::PhysicalDeviceInformation &physicalDeviceInfo,
    const rhi::DeviceStats &deviceStats, const FPSCounter &fpsCounter)
    : mPhysicalDeviceInfo(physicalDeviceInfo), mFpsCounter(fpsCounter),
      mDeviceStats(deviceStats) {}

void ImguiDebugLayer::renderMenu() {
  if (ImGui::BeginMenu("Debug")) {
    ImGui::MenuItem("Physical Device Information", nullptr,
                    &mPhysicalDeviceInfoVisible);
    ImGui::MenuItem("Usage Metrics", nullptr, &mUsageMetricsVisible);

    ImGui::MenuItem("Performance Metrics", nullptr,
                    &mPerformanceMetricsVisible);
    ImGui::EndMenu();
  }
}

void ImguiDebugLayer::render() {
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

  static const std::array<String, 3> Units{"bytes", "Kb", "Mb"};
  static constexpr float Kilo = 1024.0f;

  auto getSizeString = [](size_t size) {
    if (size < static_cast<size_t>(Kilo)) {
      return std::to_string(size) + " " + Units.at(0);
    }
    float humanReadableSize = static_cast<float>(size);

    size_t i = 1;
    for (; i < Units.size() && humanReadableSize >= Kilo; ++i) {
      humanReadableSize /= Kilo;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << humanReadableSize << " "
       << Units.at(i - 1) << " (" << size << " " << Units.at(0) << ")";
    return ss.str();
  };

  if (ImGui::BeginTable("Table", 2,
                        ImGuiTableFlags_Borders |
                            ImGuiTableColumnFlags_WidthStretch |
                            ImGuiTableFlags_RowBg)) {

    // Buffers
    renderTableRow(
        "Number of buffers",
        std::to_string(mDeviceStats.getResourceMetrics()->getBuffersCount()));

    renderTableRow(
        "Total size of allocated buffers",
        getSizeString(mDeviceStats.getResourceMetrics()->getTotalBufferSize()));

    // Textures
    renderTableRow(
        "Number of textures",
        std::to_string(mDeviceStats.getResourceMetrics()->getTexturesCount()));

    // Draw calls
    renderTableRow("Number of draw calls",
                   std::to_string(mDeviceStats.getDrawCallsCount()));
    renderTableRow("Number of drawn primitives",
                   std::to_string(mDeviceStats.getDrawnPrimitivesCount()));
    renderTableRow("Number of command calls",
                   std::to_string(mDeviceStats.getCommandCallsCount()));
    renderTableRow(
        "Number of descriptors",
        std::to_string(
            mDeviceStats.getResourceMetrics()->getDescriptorsCount()));

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

void ImguiDebugLayer::renderTableRow(StringView header, StringView value) {
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%s", String(header).c_str());
  ImGui::TableSetColumnIndex(1);
  ImGui::Text("%s", String(value).c_str());
}

} // namespace liquid
