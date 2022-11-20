#include "liquid/core/Base.h"
#include "LogViewer.h"

#include "Widgets.h"
#include "StyleStack.h"

namespace liquidator {

void LogViewer::render(LogMemoryStorage &engineLogs) {
  auto flags = ImGuiWindowFlags_HorizontalScrollbar |
               ImGuiWindowFlags_AlwaysUseWindowPadding;

  if (auto _ = widgets::Window("Logs")) {
    if (ImGui::Button("Clear")) {
      engineLogs.clear();
      mEngineLogsSize = 0;
    }

    StyleStack stack;

    ImGui::BeginChild("Engine logs",
                      ImVec2(ImGui::GetContentRegionAvail().x,
                             ImGui::GetContentRegionAvail().y),
                      false, flags);

    for (const auto &entry : engineLogs.getEntries()) {
      ImGui::Text("%s: %s",
                  liquid::getLogSeverityString(entry.severity).c_str(),
                  entry.message.c_str());
    }

    if (mEngineLogsSize != engineLogs.getEntries().size()) {
      ImGui::SetScrollHereY(1.0f);
      mEngineLogsSize = engineLogs.getEntries().size();
    }
    ImGui::EndChild();
  }
}

} // namespace liquidator
