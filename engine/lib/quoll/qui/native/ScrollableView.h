#pragma once

#include "../component/View.h"
#include "ScrollableContentView.h"
#include "ScrollbarView.h"

namespace qui {

class ScrollableView : public View {
public:
  void render() override;

  LayoutOutput layout(const LayoutInput &input) override;

  View *hitTest(const glm::vec2 &point) override;

  constexpr ScrollableContentView &getScrollableContent() {
    return mScrollableContent;
  }
  constexpr ScrollbarView &getScrollbar() { return mScrollbar; }

  constexpr void setScrollbarVisibility(bool visibility) {
    mScrollbarVisible = visibility;
  }

public:
  constexpr bool isScrollbarVisible() { return mScrollbarVisible; }

private:
  ScrollableContentView mScrollableContent;
  ScrollbarView mScrollbar;

  bool mScrollbarVisible = false;
};

} // namespace qui
