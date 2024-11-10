#include "quoll/core/Base.h"
#include "Scrollable.h"

namespace qui {

static constexpr f32 ScrollSpeed = 5.0f;

Scrollable::Scrollable(Value<Element> child) : mChild(child) {}

void Scrollable::build(BuildContext &context) {
  auto observeChildren = [this, &context] {
    auto &child = mChild();
    child.build(context);
    mView.getScrollableContent().setChild(child.getView());
  };

  mChild.observe(observeChildren);
  observeChildren();

  mEventHolder = context.eventManager->createEventHolder();

  mEventHolder.registerMouseDownHandler([this](const MouseEvent &e) {
    if (mScrollbarState == ScrollbarState::Hovered) {
      setScrollbarState(ScrollbarState::Active);

      mPreviousMousePos = e.point;
    }
  });

  mEventHolder.registerMouseUpHandler([this](const MouseEvent &e) {
    auto &scrollBar = mView.getScrollbar();

    if (mScrollbarState == ScrollbarState::Active) {
      if (mHovered) {
        HitTestResult hitResult{};
        if (scrollBar.hitTest(e.point, hitResult)) {
          setScrollbarState(ScrollbarState::Hovered);
        } else {
          setScrollbarState(ScrollbarState::Visible);
        }
      } else {
        setScrollbarState(ScrollbarState::Hidden);
      }
    }
  });

  mEventHolder.registerMouseMoveHandler([this](const MouseEvent &e) {
    auto &content = mView.getScrollableContent();
    auto &scrollBar = mView.getScrollbar();

    {
      HitTestResult hitResult{};
      mHovered = content.hitTest(e.point, hitResult);
    }

    if (mScrollbarState == ScrollbarState::Active) {
      if (e.point.y >= content.getPosition().y &&
          e.point.y <= content.getPosition().y + content.getSize().y) {
        const f32 deltaY = e.point.y - mPreviousMousePos.y;
        content.scroll({0.0f, -deltaY});
      }

      mPreviousMousePos = e.point;
    } else if (mHovered) {
      HitTestResult hitResult;
      if (scrollBar.hitTest(e.point, hitResult)) {
        setScrollbarState(ScrollbarState::Hovered);
      } else {
        setScrollbarState(ScrollbarState::Visible);
      }
    } else {
      setScrollbarState(ScrollbarState::Hidden);
    }
  });

  mEventHolder.registerMouseWheelHandler([this](const MouseWheelEvent &event) {
    if (mHovered && mScrollbarState != ScrollbarState::Active) {
      mView.getScrollableContent().scroll(event.delta * ScrollSpeed);
    }
  });
}

void Scrollable::setScrollbarState(ScrollbarState state) {
  static constexpr f32 ThickScrollbar = 12.0f;
  static constexpr f32 ThinScrollbar = 6.0f;

  mScrollbarState = state;

  mView.setScrollbarVisibility(state != ScrollbarState::Hidden);

  if (mScrollbarState == ScrollbarState::Hidden) {
    return;
  }

  auto &scrollbar = mView.getScrollbar();
  if (state > ScrollbarState::Visible) {
    scrollbar.setThickness(ThickScrollbar);
    scrollbar.setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
  } else {
    scrollbar.setThickness(ThinScrollbar);
    const Color color(0.0f, 0.0f, 0.0f, 0.9f);
    scrollbar.setColor(color);
  }
}

} // namespace qui
