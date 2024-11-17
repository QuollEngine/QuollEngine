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
  auto *drawList = ImGui::GetWindowDrawList();

  drawList->PushClipRect({mPosition.x, mPosition.y},
                         {mPosition.x + mSize.x, mPosition.y + mSize.y}, true);
  mChild->render();
  drawList->PopClipRect();
}

void ScrollableContentView::layout(const LayoutInput &input) {
  mPosition = input.position;

  mChild->layout({Constraints(), mPosition + mScrollOffset});
  mContentSize = mChild->getSize();

  if (input.constraints.max.x < Constraints::Infinity &&
      input.constraints.max.y < Constraints::Infinity) {
    mSize = input.constraints.max;
  } else {
    mSize = input.constraints.clamp(mContentSize);
  }
}

bool ScrollableContentView::hitTest(const glm::vec2 &point,
                                    HitTestResult &hitResult) {
  if (isPointInBounds(point)) {
    hitResult.path.push_back(this);
    mChild->hitTest(point, hitResult);
    return true;
  }

  return false;
}

} // namespace qui
