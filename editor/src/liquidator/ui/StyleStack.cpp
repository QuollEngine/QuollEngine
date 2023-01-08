#include "liquid/core/Base.h"
#include "StyleStack.h"

#include "liquid/imgui/Imgui.h"

namespace liquidator {

StyleStack::~StyleStack() {
  if (mPushedColors > 0) {
    ImGui::PopStyleColor(static_cast<int>(mPushedColors));
    mPushedColors = 0;
  }

  if (mPushedStyles > 0) {
    ImGui::PopStyleVar(static_cast<int>(mPushedStyles));
    mPushedStyles = 0;
  }

  for (uint32_t i = 0; i < mPushedFonts; ++i) {
    ImGui::PopFont();
  }
}

void StyleStack::pushColor(uint32_t colorIndex, const glm::vec4 &color) {
  pushColor(colorIndex, ImVec4(color.x, color.y, color.z, color.w));
}

void StyleStack::pushColor(uint32_t colorIndex, const ImVec4 &color) {
  ImGui::PushStyleColor(static_cast<ImGuiCol>(colorIndex), color);
  mPushedColors++;
}

void StyleStack::pushStyle(uint32_t styleIndex, float value) {
  ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(styleIndex), value);
  mPushedStyles++;
}

void StyleStack::pushStyle(uint32_t styleIndex, const glm::vec2 &value) {
  pushStyle(styleIndex, ImVec2(value.x, value.y));
}

void StyleStack::pushStyle(uint32_t styleIndex, const ImVec2 &value) {
  ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(styleIndex), value);
  mPushedStyles++;
}

void StyleStack::pushFont(ImFont *font) {
  ImGui::PushFont(font);
  mPushedFonts++;
}

} // namespace liquidator
