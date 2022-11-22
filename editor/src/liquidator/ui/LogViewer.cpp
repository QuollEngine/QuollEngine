#include "liquid/core/Base.h"
#include "LogViewer.h"

#include "Widgets.h"
#include "StyleStack.h"

namespace liquidator {

void LogViewer::render(LogMemoryStorage &systemLogs,
                       LogMemoryStorage &userLogs) {
  auto flags = ImGuiWindowFlags_HorizontalScrollbar |
               ImGuiWindowFlags_AlwaysUseWindowPadding;

  if (auto _ = widgets::Window("Logs")) {
    const float FirstHalf = ImGui::GetContentRegionAvail().x / 2.0f;

    renderLogContainer("System Logs", systemLogs, mSystemLogSize, FirstHalf);
    ImGui::SameLine();
    renderLogContainer("User logs", userLogs, mUserLogSize,
                       ImGui::GetContentRegionAvail().x);
  }
}

void LogViewer::renderLogContainer(const liquid::String &name,
                                   LogMemoryStorage &logStorage,
                                   size_t &logSize, float width) {
  ImGui::PushID(name.c_str());
  ImGui::BeginGroup();
  auto flags = ImGuiWindowFlags_HorizontalScrollbar |
               ImGuiWindowFlags_AlwaysUseWindowPadding;

  if (ImGui::Button("Clear")) {
    logStorage.clear();
    logSize = 0;
  }

  StyleStack stack;

  ImGui::BeginChild(name.c_str(),
                    ImVec2(width, ImGui::GetContentRegionAvail().y), false,
                    flags);

  for (const auto &entry : logStorage.getEntries()) {
    ImGui::Text("%s: %s", liquid::getLogSeverityString(entry.severity).c_str(),
                entry.message.c_str());
  }

  if (logSize != logStorage.getEntries().size()) {
    ImGui::SetScrollHereY(1.0f);
    logSize = logStorage.getEntries().size();
  }
  ImGui::EndChild();
  ImGui::EndGroup();
  ImGui::PopID();
}

} // namespace liquidator
