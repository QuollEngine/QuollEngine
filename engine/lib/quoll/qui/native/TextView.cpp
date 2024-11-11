#include "quoll/core/Base.h"
#include "TextView.h"
#include <imgui.h>

namespace qui {

void TextView::render() {
  auto *drawList = ImGui::GetWindowDrawList();

  drawList->AddText(
      nullptr, 0.0f, ImVec2(mPosition.x, mPosition.y),
      ImColor(mColor.value.x, mColor.value.y, mColor.value.z, mColor.value.w),
      mText.c_str(), nullptr, mSize.x);
}

void TextView::layout(const LayoutInput &input) {
  const auto &constraints = input.constraints;
  auto size =
      ImGui::CalcTextSize(mText.c_str(), nullptr, true, constraints.max.x);

  mSize = constraints.clamp({size.x, size.y});
  mPosition = input.position;
}

bool TextView::hitTest(const glm::vec2 &point, HitTestResult &hitResult) {
  if (isPointInBounds(point)) {
    hitResult.path.push_back(this);
    return true;
  }

  return false;
}

} // namespace qui
