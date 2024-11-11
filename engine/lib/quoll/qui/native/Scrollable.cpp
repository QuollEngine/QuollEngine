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

  mView.getScrollbar().getEventDispatcher().registerMouseDownHandler(
      [this](const auto &e) {
        if (!mScrollbarActive) {
          setScrollbarActive(true);
          mPreviousMousePos = e.point;
        }
      });

  mView.getScrollbar().getEventDispatcher().registerMouseUpHandler(
      [this](const MouseEvent &e) { setScrollbarActive(false); });

  mEventHolder.registerMouseMoveHandler([this](const MouseEvent &e) {
    if (!mScrollbarActive) {
      return;
    }

    auto &content = mView.getScrollableContent();
    if (e.point.y >= content.getPosition().y &&
        e.point.y <= content.getPosition().y + content.getSize().y) {
      const f32 deltaY = e.point.y - mPreviousMousePos.y;
      content.scroll({0.0f, -deltaY});
    }

    mPreviousMousePos = e.point;
  });

  mView.getScrollbar().getEventDispatcher().registerMouseEnterHandler(
      [this](const auto &) { setScrollbarState(ScrollbarState::Hovered); });

  mView.getScrollbar().getEventDispatcher().registerMouseExitHandler(
      [this](const auto &) { setScrollbarState(ScrollbarState::Hidden); });

  mView.getScrollableContent().getEventDispatcher().registerMouseEnterHandler(
      [this](const auto &) {
        if (mScrollbarState == ScrollbarState::Hidden) {
          setScrollbarState(ScrollbarState::Visible);
        }
      });

  mView.getScrollableContent().getEventDispatcher().registerMouseExitHandler(
      [this](const auto &) { setScrollbarState(ScrollbarState::Hidden); });

  mView.getScrollableContent().getEventDispatcher().registerMouseWheelHandler(
      [this](const MouseWheelEvent &event) {
        if (!mScrollbarActive) {
          mView.getScrollableContent().scroll(event.delta * ScrollSpeed);
        }
      });
}

void Scrollable::setScrollbarActive(bool isActive) {
  mScrollbarActive = isActive;
  setScrollbarState(mScrollbarState);
}

void Scrollable::setScrollbarState(ScrollbarState state) {
  static constexpr f32 ThickScrollbar = 12.0f;
  static constexpr f32 ThinScrollbar = 6.0f;

  mScrollbarState = state;

  const bool isScrollbarVisible =
      mScrollbarActive || state != ScrollbarState::Hidden;

  mView.setScrollbarVisibility(isScrollbarVisible);

  if (!isScrollbarVisible) {
    return;
  }

  auto &scrollbar = mView.getScrollbar();
  if (state == ScrollbarState::Hovered || mScrollbarActive) {
    scrollbar.setThickness(ThickScrollbar);
    scrollbar.setColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
  } else {
    scrollbar.setThickness(ThinScrollbar);
    const Color color(0.0f, 0.0f, 0.0f, 0.9f);
    scrollbar.setColor(color);
  }
}

} // namespace qui
