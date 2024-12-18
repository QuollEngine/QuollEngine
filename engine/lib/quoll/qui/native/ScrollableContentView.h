#pragma once

#include "../component/View.h"

namespace qui {

class ScrollableContentView : public View {
public:
  constexpr void setChild(View *child) { mChild = child; }

  void scroll(const glm::vec2 &offset);

  void render() override;
  void layout(const LayoutInput &input) override;
  bool hitTest(const glm::vec2 &point, HitTestResult &hitResult) override;

public:
  constexpr const auto &getScrollOffset() const { return mScrollOffset; }
  constexpr auto *getChild() const { return mChild; }
  constexpr const auto &getContentSize() const { return mContentSize; }

private:
  glm::vec2 mScrollOffset{0.0f, 0.0f};
  View *mChild = nullptr;

  glm::vec2 mContentSize{0.0f, 0.0f};
};

} // namespace qui
