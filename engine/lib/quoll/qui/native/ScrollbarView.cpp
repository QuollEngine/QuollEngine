#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "ScrollbarView.h"

namespace qui {

void ScrollbarView::render() {
  auto *drawList = ImGui::GetWindowDrawList();
  drawList->AddRectFilled(
      ImVec2(mPosition.x, mPosition.y),
      ImVec2(mPosition.x + mSize.x, mPosition.y + mSize.y),
      ImColor(mColor.value.r, mColor.value.g, mColor.value.b, mColor.value.a),
      mBorderRadius);
}

void ScrollbarView::layout(const LayoutInput &input) {
  mPosition = input.position;
  mSize = input.constraints.clamp({0.0f, 0.0f});

  mPosition.x -= mThickness;
  mSize.x = mThickness;
}

bool ScrollbarView::hitTest(const glm::vec2 &point, HitTestResult &hitResult) {
  if (isPointInBounds(point)) {
    hitResult.path.push_back(this);
    return true;
  }

  return false;
}

} // namespace qui
