#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "StyleStack.h"

namespace quoll::editor {

StyleStack::~StyleStack() {
  if (mPushedColors > 0) {
    ImGui::PopStyleColor(static_cast<int>(mPushedColors));
    mPushedColors = 0;
  }

  if (mPushedStyles > 0) {
    ImGui::PopStyleVar(static_cast<int>(mPushedStyles));
    mPushedStyles = 0;
  }

  for (u32 i = 0; i < mPushedFonts; ++i) {
    ImGui::PopFont();
  }
}

void StyleStack::pushColor(u32 colorIndex, const glm::vec4 &color) {
  pushColor(colorIndex, ImVec4(color.x, color.y, color.z, color.w));
}

void StyleStack::pushColor(u32 colorIndex, const ImVec4 &color) {
  ImGui::PushStyleColor(static_cast<ImGuiCol>(colorIndex), color);
  mPushedColors++;
}

void StyleStack::pushStyle(u32 styleIndex, f32 value) {
  ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(styleIndex), value);
  mPushedStyles++;
}

void StyleStack::pushStyle(u32 styleIndex, const glm::vec2 &value) {
  pushStyle(styleIndex, ImVec2(value.x, value.y));
}

void StyleStack::pushStyle(u32 styleIndex, const ImVec2 &value) {
  ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(styleIndex), value);
  mPushedStyles++;
}

void StyleStack::pushFont(ImFont *font) {
  ImGui::PushFont(font);
  mPushedFonts++;
}

} // namespace quoll::editor
