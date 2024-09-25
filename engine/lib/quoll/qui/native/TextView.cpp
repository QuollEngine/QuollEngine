#include "quoll/core/Base.h"
#include "TextView.h"
#include <imgui.h>

namespace qui {

void TextView::render() {
  auto *drawList = ImGui::GetWindowDrawList();

  drawList->AddText(
      nullptr, 0.0f, ImVec2(mPosition.x, mPosition.y),
      ImColor(mColor.value.x, mColor.value.y, mColor.value.z, mColor.value.w),
      mText.c_str(), nullptr, mTextSize.x);
}

LayoutOutput TextView::layout(const LayoutInput &input) {
  const auto &constraints = input.constraints;
  auto size =
      ImGui::CalcTextSize(mText.c_str(), nullptr, true, constraints.maxWidth);

  mTextSize = {constraints.clampWidth(size.x), constraints.clampHeight(size.y)};

  mPosition = input.position;

  return LayoutOutput{mTextSize};
}

} // namespace qui
