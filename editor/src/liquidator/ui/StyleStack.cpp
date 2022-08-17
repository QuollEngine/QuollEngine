#include "liquid/core/Base.h"
#include "StyleStack.h"

#include "liquid/imgui/Imgui.h"

namespace liquidator {

StyleStack::~StyleStack() {
  if (mPushedColors > 0) {
    ImGui::PopStyleColor(static_cast<int>(mPushedColors));
    mPushedColors = 0;
  }
}

void StyleStack::pushColor(uint32_t colorIndex, const glm::vec4 &color) {
  ImGui::PushStyleColor(static_cast<ImGuiCol>(colorIndex),
                        ImVec4(color.x, color.y, color.z, color.w));
  mPushedColors++;
}

} // namespace liquidator
