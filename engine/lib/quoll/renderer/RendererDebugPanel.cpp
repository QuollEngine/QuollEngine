#include "quoll/core/Base.h"
#include "quoll/imgui/ImguiUtils.h"
#include "quoll/rhi/RenderDevice.h"
#include "RendererDebugPanel.h"

namespace quoll::debug {

namespace {

void renderTableRow(StringView header, StringView value) {
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%s", String(header).c_str());
  ImGui::TableSetColumnIndex(1);
  ImGui::Text("%s", String(value).c_str());
}

} // namespace

RendererDebugPanel::RendererDebugPanel(rhi::RenderDevice *device)
    : mDevice(device) {}

void RendererDebugPanel::onRenderMenu() {
  ImGui::MenuItem("Physical Device Information", nullptr,
                  &mPhysicalDeviceInfoOpen);
  ImGui::MenuItem("Usage Metrics", nullptr, &mUsageMetricsOpen);
}

void RendererDebugPanel::onRender() {
  renderPhysicalDeviceInfo();
  renderUsageMetrics();
}

void RendererDebugPanel::renderPhysicalDeviceInfo() {
  if (!mPhysicalDeviceInfoOpen)
    return;

  const auto &deviceInfo = mDevice->getDeviceInformation();

  if (ImGui::Begin(("Device Info: " + deviceInfo.getName()).c_str(),
                   &mPhysicalDeviceInfoOpen, ImGuiWindowFlags_NoDocking)) {
    if (ImGui::BeginTable("Table", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame)) {
      renderTableRow("Name", deviceInfo.getName());
      renderTableRow("Type", deviceInfo.getTypeString());

      ImGui::EndTable();
    }

    ImGui::End();
  }
}

void RendererDebugPanel::renderUsageMetrics() {
  if (!mUsageMetricsOpen)
    return;

  if (ImGui::Begin("Usage Metrics", &mUsageMetricsOpen,
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

    const auto &deviceStats = mDevice->getDeviceStats();
    if (ImGui::BeginTable("Table", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {
      // Buffers
      renderTableRow(
          "Number of buffers",
          std::to_string(deviceStats.getResourceMetrics()->getBuffersCount()));

      renderTableRow(
          "Total size of allocated buffers",
          getSizeString(
              deviceStats.getResourceMetrics()->getTotalBufferSize()));

      // Textures
      renderTableRow(
          "Number of textures",
          std::to_string(deviceStats.getResourceMetrics()->getTexturesCount()));

      // Draw calls
      renderTableRow("Number of draw calls",
                     std::to_string(deviceStats.getDrawCallsCount()));
      renderTableRow("Number of drawn primitives",
                     std::to_string(deviceStats.getDrawnPrimitivesCount()));
      renderTableRow("Number of command calls",
                     std::to_string(deviceStats.getCommandCallsCount()));
      renderTableRow(
          "Number of descriptors",
          std::to_string(
              deviceStats.getResourceMetrics()->getDescriptorsCount()));

      ImGui::EndTable();
    }

    ImGui::End();
  }
}

} // namespace quoll::debug
