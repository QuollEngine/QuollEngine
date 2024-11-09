#include "quoll/core/Base.h"
#include "quoll/qui/native/Scrollable.h"
#include "quoll/qui/reactive/Scope.h"
#include "MockComponent.h"
#include "QuiComponentTest.h"

class QuiScrollableTest : public QuiComponentTest {
public:
  QuiScrollableTest() : child(20) {
    child.mView.desiredSize = {500.0f, 600.0f};
  }

public:
  MockComponent child;
};

TEST_F(QuiScrollableTest, MouseWheelDoesNothingIfComponentIsNotHovered) {
  qui::Element element = qui::Scrollable(child);
  element.build(buildContext);
  element.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(element.getView());

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));

  dispatchMouseWheelEvent({glm::vec2{-20.0f, -30.0f}});

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));
}

TEST_F(QuiScrollableTest, MouseWheelDoesNothingIfScrollbarIsDragging) {
  qui::Element element = qui::Scrollable(child);
  element.build(buildContext);
  element.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(element.getView());

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));

  dispatchMouseMoveEvent({glm::vec2{195.0f, 20.0f}});
  dispatchMouseDownEvent({glm::vec2{195.0f, 20.0f}});
  dispatchMouseWheelEvent({glm::vec2{-20.0f, -30.0f}});

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));
}

TEST_F(
    QuiScrollableTest,
    MouseWheelUpdatesScrollOffsetIfComponentIsHoveredAndScrollbarIsNotDragging) {
  qui::Element element = qui::Scrollable(child);
  element.build(buildContext);
  element.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(element.getView());

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));

  dispatchMouseMoveEvent({glm::vec2{150.0f, 150.0f}});
  dispatchMouseWheelEvent({glm::vec2{-20.0f, -30.0f}});

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(-100.0f, -150.0f));
}

TEST_F(QuiScrollableTest, ScrollbarIsNotVisibleWhenComponentIsNotHovered) {
  qui::Element element = qui::Scrollable(child);
  element.build(buildContext);
  element.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(element.getView());

  EXPECT_FALSE(view->isScrollbarVisible());

  dispatchMouseMoveEvent({glm::vec2{1000.0, 0.0f}});
  EXPECT_FALSE(view->isScrollbarVisible());
}

TEST_F(QuiScrollableTest, ThinScrollbarIsVisibleWhenComponentIsHovered) {
  qui::Element element = qui::Scrollable(child);
  element.build(buildContext);
  element.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(element.getView());

  dispatchMouseMoveEvent({glm::vec2{150.0f, 150.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 6.0f);
}

TEST_F(QuiScrollableTest, ThickScrollbarIsVisibleWhenScrollbarIsHovered) {
  qui::Element element = qui::Scrollable(child);
  element.build(buildContext);
  element.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(element.getView());

  dispatchMouseMoveEvent({glm::vec2{195.0f, 20.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 12.0f);
}

TEST_F(QuiScrollableTest, ScrollbarStaysActiveIfDraggingStarted) {
  qui::Element element = qui::Scrollable(child);
  element.build(buildContext);
  element.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(element.getView());

  dispatchMouseMoveEvent({glm::vec2{195.0f, 20.0f}});
  dispatchMouseDownEvent({glm::vec2{195.0f, 20.0f}});

  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 12.0f);

  dispatchMouseMoveEvent({glm::vec2{1000.0f, 1000.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 12.0f);
}

TEST_F(QuiScrollableTest,
       EndingScrollbarDragSetsScrollbarStateBasedOnMousePosition) {
  qui::Element element = qui::Scrollable(child);
  element.build(buildContext);
  element.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(element.getView());

  dispatchMouseMoveEvent({glm::vec2{195.0f, 20.0f}});

  // Hovered state
  dispatchMouseMoveEvent({glm::vec2{195.0f, 20.0f}});
  dispatchMouseDownEvent({glm::vec2{195.0f, 20.0f}});
  dispatchMouseUpEvent({glm::vec2{195.0f, 20.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 12.0f);

  // Visible state
  dispatchMouseMoveEvent({glm::vec2{195.0f, 20.0f}});
  dispatchMouseDownEvent({glm::vec2{195.0f, 20.0f}});
  dispatchMouseMoveEvent({glm::vec2{150.0f, 150.0f}});
  dispatchMouseUpEvent({glm::vec2{150.0f, 150.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 6.0f);

  // Hidden state
  dispatchMouseMoveEvent({glm::vec2{195.0f, 20.0f}});
  dispatchMouseDownEvent({glm::vec2{195.0f, 20.0f}});
  dispatchMouseMoveEvent({glm::vec2{1000.0, 1000.0f}});
  dispatchMouseUpEvent({glm::vec2{1000.0f, 1000.0f}});
  EXPECT_FALSE(view->isScrollbarVisible());
}
