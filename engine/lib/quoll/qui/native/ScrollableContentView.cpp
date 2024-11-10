#include "quoll/core/Base.h"
#include "ScrollableContentView.h"
#include <imgui.h>

namespace qui {

void ScrollableContentView::scroll(const glm::vec2 &offset) {
  mScrollOffset.x =
      glm::clamp(mScrollOffset.x + offset.x, -(mContentSize.x - mSize.x), 0.0f);
  mScrollOffset.y =
      glm::clamp(mScrollOffset.y + offset.y, -(mContentSize.y - mSize.y), 0.0f);
}

void ScrollableContentView::render() {
  ImGui::PushClipRect({mPosition.x, mPosition.y},
                      {mPosition.x + mSize.x, mPosition.y + mSize.y}, true);
  mChild->render();
  ImGui::PopClipRect();
}

LayoutOutput ScrollableContentView::layout(const LayoutInput &input) {
  mPosition = input.position;

  auto childOutput = mChild->layout({Constraints(), mPosition + mScrollOffset});
  mContentSize = childOutput.size;

  if (input.constraints.max.x < Constraints::Infinity &&
      input.constraints.max.y < Constraints::Infinity) {
    mSize = input.constraints.max;
  } else {
    mSize = input.constraints.clamp(mContentSize);
  }

  return {mSize};
}

bool ScrollableContentView::hitTest(const glm::vec2 &point,
                                    HitTestResult &hitResult) {
  if (point.x >= mPosition.x && point.x <= mPosition.x + mSize.x &&
      point.y >= mPosition.y && point.y <= mPosition.y + mSize.y) {
    hitResult.path.push_back(this);
    mChild->hitTest(point, hitResult);
    return true;
  }

  return false;
}

} // namespace qui
