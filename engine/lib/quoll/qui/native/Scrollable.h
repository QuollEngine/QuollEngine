#pragma once

#include "../component/Element.h"
#include "../reactive/Value.h"
#include "ScrollableView.h"

namespace qui {

class Scrollable : public Component {
  enum class ScrollbarState { Hidden = 0, Visible, Hovered, Active };

public:
  Scrollable(Value<Element> child);

  void build(BuildContext &context) override;

  constexpr View *getView() override { return &mView; }

private:
  void setScrollbarState(ScrollbarState state);

private:
  ScrollableView mView;
  EventHolder mEventHolder;

  Value<Element> mChild;
  bool mHovered = false;

  glm::vec2 mPreviousMousePos{0.0f, 0.0f};

  ScrollbarState mScrollbarState = ScrollbarState::Hidden;
};

} // namespace qui
