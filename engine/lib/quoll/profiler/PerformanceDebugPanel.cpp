#include "quoll/core/Base.h"
#include "quoll/imgui/ImguiUtils.h"
#include "FPSCounter.h"
#include "MetricsCollector.h"
#include "PerformanceDebugPanel.h"

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

PerformanceDebugPanel::PerformanceDebugPanel(rhi::RenderDevice *device,
                                             MetricsCollector &metricsCollector,
                                             const FPSCounter &fpsCounter)
    : mDevice(device), mMetricsCollector(&metricsCollector),
      mFpsCounter(&fpsCounter) {}

void PerformanceDebugPanel::onRenderMenu() {
  ImGui::MenuItem("Performance Metrics", nullptr, &mOpen);
}

void PerformanceDebugPanel::onRender() {
  static constexpr u32 OneSecondInMs = 1000;
  if (!mOpen)
    return;

  const u32 fps = mFpsCounter->getFPS();

  if (ImGui::Begin("Performance Metrics", &mOpen, ImGuiWindowFlags_NoDocking)) {
    if (ImGui::BeginTable("Overview", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {

      renderTableRow("FPS", std::to_string(fps));
      renderTableRow("Frame time",
                     std::to_string(fps > 0 ? OneSecondInMs / fps : 0) + "ms");
      ImGui::EndTable();
    }

    if (ImGui::BeginTable("GPU Timings", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {
      const auto period =
          mDevice->getDeviceInformation().getLimits().timestampPeriod /
          1000000.0f;

      for (const auto &metric : mMetricsCollector->measure(period)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", metric.label.c_str());
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f ms", metric.value);
      }

      ImGui::EndTable();
    }
    ImGui::End();
  }
}

} // namespace quoll::debug
