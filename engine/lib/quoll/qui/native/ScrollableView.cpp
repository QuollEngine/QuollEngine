#include "quoll/core/Base.h"
#include "ScrollableView.h"

namespace qui {

void ScrollableView::render() {
  mScrollableContent.render();

  if (mScrollbarVisible) {
    mScrollbar.render();
  }
}

LayoutOutput ScrollableView::layout(const LayoutInput &input) {
  const auto output = mScrollableContent.layout(input);

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

  return output;
}

View *ScrollableView::hitTest(const glm::vec2 &point) {
  auto *view = mScrollbar.hitTest(point);
  if (view) {
    return view;
  }

  return mScrollableContent.hitTest(point);
}

} // namespace qui
