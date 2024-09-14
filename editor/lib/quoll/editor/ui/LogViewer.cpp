#include "quoll/core/Base.h"
#include "LogViewer.h"
#include "StyleStack.h"
#include "Widgets.h"

namespace quoll::editor {

void LogViewer::render(LogMemoryStorage &userLogs) {
  auto flags = ImGuiWindowFlags_HorizontalScrollbar |
               ImGuiWindowFlags_AlwaysUseWindowPadding;

  if (auto _ = widgets::Window("Logs")) {
    renderLogContainer("User logs", userLogs, mUserLogSize,
                       ImGui::GetContentRegionAvail().x);
  }
}

void LogViewer::renderLogContainer(const String &name,
                                   LogMemoryStorage &logStorage, usize &logSize,
                                   f32 width) {
  ImGui::PushID(name.c_str());
  ImGui::BeginGroup();
  auto flags = ImGuiWindowFlags_HorizontalScrollbar |
               ImGuiWindowFlags_AlwaysUseWindowPadding;

  if (widgets::Button("Clear")) {
    logStorage.clear();
    logSize = 0;
  }

  StyleStack stack;

  ImGui::BeginChild(name.c_str(),
                    ImVec2(width, ImGui::GetContentRegionAvail().y), false,
                    flags);

  for (const auto &entry : logStorage.getEntries()) {
    ImGui::Text("%s: %s", getLogSeverityString(entry.severity).c_str(),
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

} // namespace quoll::editor
