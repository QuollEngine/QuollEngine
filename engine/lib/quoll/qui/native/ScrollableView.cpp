#include "quoll/core/Base.h"
#include "ScrollableView.h"

namespace qui {

void ScrollableView::render() {
  mScrollableContent.render();

  if (mScrollbarVisible) {
    mScrollbar.render();
  }
}

void ScrollableView::layout(const LayoutInput &input) {
  mScrollableContent.layout(input);

  mSize = mScrollableContent.getSize();
  mPosition = mScrollableContent.getPosition();

  const auto containerSize = mScrollableContent.getSize().y;
  const auto contentSize = mScrollableContent.getContentSize().y;
  const auto offset = mScrollableContent.getScrollOffset().y;

  const auto scrollbarSize = (containerSize / contentSize) * containerSize;
  const auto scrollbarPosition = (-offset / (contentSize - containerSize)) *
                                 (containerSize - scrollbarSize);

  const auto position =
      mScrollableContent.getPosition() +
      glm::vec2(mScrollableContent.getSize().x, scrollbarPosition);

  mScrollbar.layout(
      {Constraints(0.0f, scrollbarSize, Constraints::Infinity, scrollbarSize),
       position});
}

bool ScrollableView::hitTest(const glm::vec2 &point, HitTestResult &hitResult) {
  const bool hitScrollbar = mScrollbar.hitTest(point, hitResult);
  const bool hitContent = mScrollableContent.hitTest(point, hitResult);

  return hitScrollbar || hitContent;
}

} // namespace qui
