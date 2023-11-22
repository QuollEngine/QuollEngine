#include "quoll/core/Base.h"
#include "quoll/core/Property.h"
#include "ImguiDebugLayer.h"
#include "quoll/imgui/Imgui.h"

namespace quoll {

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
    ImGui::MenuItem("Imgui demo", nullptr, &mDemoWindowVisible);
    ImGui::EndMenu();
  }
}

void ImguiDebugLayer::render() {
  renderPhysicalDeviceInfo();
  renderUsageMetrics();
  renderPerformanceMetrics();
  renderDemoWindow();
}

void ImguiDebugLayer::renderPerformanceMetrics() {
  const u32 ONE_SECOND_IN_MS = 1000;
  if (!mPerformanceMetricsVisible)
    return;

  u32 fps = mFpsCounter.getFPS();

  if (ImGui::Begin("Performance Metrics", &mPerformanceMetricsVisible,
                   ImGuiWindowFlags_NoDocking)) {
    if (ImGui::BeginTable("Table", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {

      renderTableRow("FPS", std::to_string(fps));
      renderTableRow("Frame time",
                     std::to_string(fps > 0 ? ONE_SECOND_IN_MS / fps : 0) +
                         "ms");
      ImGui::EndTable();
    }
    ImGui::End();
  }
}

void ImguiDebugLayer::renderUsageMetrics() {
  if (!mUsageMetricsVisible)
    return;

  if (ImGui::Begin("Usage Metrics", &mUsageMetricsVisible,
                   ImGuiWindowFlags_NoDocking)) {

    static const std::array<String, 3> Units{"bytes", "Kb", "Mb"};
    static constexpr f32 Kilo = 1024.0f;

    auto getSizeString = [](usize size) {
      if (size < static_cast<usize>(Kilo)) {
        return std::to_string(size) + " " + Units.at(0);
      }
      f32 humanReadableSize = static_cast<f32>(size);

      usize i = 1;
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
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {
      // Buffers
      renderTableRow(
          "Number of buffers",
          std::to_string(mDeviceStats.getResourceMetrics()->getBuffersCount()));

      renderTableRow(
          "Total size of allocated buffers",
          getSizeString(
              mDeviceStats.getResourceMetrics()->getTotalBufferSize()));

      // Textures
      renderTableRow(
          "Number of textures",
          std::to_string(
              mDeviceStats.getResourceMetrics()->getTexturesCount()));

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
}

void ImguiDebugLayer::renderDemoWindow() {
  if (!mDemoWindowVisible)
    return;

  ImGui::ShowDemoWindow(&mDemoWindowVisible);
}

void ImguiDebugLayer::renderPhysicalDeviceInfo() {
  if (!mPhysicalDeviceInfoVisible)
    return;

  if (ImGui::Begin(("Device Info: " + mPhysicalDeviceInfo.getName()).c_str(),
                   &mPhysicalDeviceInfoVisible, ImGuiWindowFlags_NoDocking)) {
    if (ImGui::BeginTable("Table", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame)) {
      renderTableRow("Name", mPhysicalDeviceInfo.getName());
      renderTableRow("Type", mPhysicalDeviceInfo.getTypeString());

      ImGui::EndTable();
    }

    ImGui::End();
  }
}

void ImguiDebugLayer::renderTableRow(StringView header, StringView value) {
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%s", String(header).c_str());
  ImGui::TableSetColumnIndex(1);
  ImGui::Text("%s", String(value).c_str());
}

} // namespace quoll
