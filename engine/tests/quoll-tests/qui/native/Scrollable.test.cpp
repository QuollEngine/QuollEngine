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
  auto tree = qui::Qui::createTree(qui::Scrollable(child));
  MockEventController events(tree);

  tree.root.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {100.0f, 100.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(tree.root.getView());

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));

  events.mouseWheel({glm::vec2{-20.0f, -30.0f}});

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));
}

TEST_F(QuiScrollableTest, MouseWheelDoesNothingIfScrollbarIsDragging) {
  auto tree = qui::Qui::createTree(qui::Scrollable(child));
  MockEventController events(tree);

  tree.root.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(tree.root.getView());

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));

  events.mouseDown({glm::vec2{195.0f, 20.0f}});
  events.mouseWheel({glm::vec2{-20.0f, -30.0f}});

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));
}

TEST_F(
    QuiScrollableTest,
    MouseWheelUpdatesScrollOffsetIfComponentIsHoveredAndScrollbarIsNotDragging) {
  auto tree = qui::Qui::createTree(qui::Scrollable(child));
  MockEventController events(tree);

  tree.root.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(tree.root.getView());

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(0.0f, 0.0f));

  events.mouseMove({glm::vec2{150.0f, 150.0f}});
  events.mouseWheel({glm::vec2{-20.0f, -30.0f}});

  EXPECT_EQ(view->getScrollableContent().getScrollOffset(),
            glm::vec2(-100.0f, -150.0f));
}

TEST_F(QuiScrollableTest, ScrollbarIsNotVisibleWhenComponentIsNotHovered) {
  auto tree = qui::Qui::createTree(qui::Scrollable(child));
  MockEventController events(tree);

  tree.root.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(tree.root.getView());

  EXPECT_FALSE(view->isScrollbarVisible());

  events.mouseMove({glm::vec2{1000.0, 0.0f}});
  EXPECT_FALSE(view->isScrollbarVisible());
}

TEST_F(QuiScrollableTest, ThinScrollbarIsVisibleWhenComponentIsHovered) {
  auto tree = qui::Qui::createTree(qui::Scrollable(child));
  MockEventController events(tree);

  tree.root.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(tree.root.getView());

  events.mouseMove({glm::vec2{150.0f, 150.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 6.0f);
}

TEST_F(QuiScrollableTest, ThickScrollbarIsVisibleWhenScrollbarIsHovered) {
  auto tree = qui::Qui::createTree(qui::Scrollable(child));
  MockEventController events(tree);

  tree.root.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(tree.root.getView());

  events.mouseMove({glm::vec2{195.0f, 20.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 12.0f);
}

TEST_F(QuiScrollableTest, ScrollbarStaysActiveIfDraggingStarted) {
  auto tree = qui::Qui::createTree(qui::Scrollable(child));
  MockEventController events(tree);

  tree.root.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(tree.root.getView());

  events.mouseDown({195.0f, 20.0f});

  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 12.0f);

  events.mouseMove({glm::vec2{1000.0f, 1000.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 12.0f);
}

TEST_F(QuiScrollableTest,
       EndingScrollbarDragSetsScrollbarStateBasedOnMousePosition) {
  auto tree = qui::Qui::createTree(qui::Scrollable(child));
  MockEventController events(tree);

  tree.root.getView()->layout(
      {qui::Constraints(200.0f, 300.0f, 200.0f, 300.0f), {0.0f, 0.0f}});
  auto *view = dynamic_cast<qui::ScrollableView *>(tree.root.getView());

  events.mouseMove({glm::vec2{195.0f, 20.0f}});

  // Hovered state
  events.mouseMove({glm::vec2{195.0f, 20.0f}});
  events.mouseDown({glm::vec2{195.0f, 20.0f}});
  events.mouseUp({glm::vec2{195.0f, 20.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 12.0f);

  // Visible state
  events.mouseMove({glm::vec2{195.0f, 20.0f}});
  events.mouseDown({glm::vec2{195.0f, 20.0f}});
  events.mouseMove({glm::vec2{150.0f, 150.0f}});
  events.mouseUp({glm::vec2{150.0f, 150.0f}});
  EXPECT_TRUE(view->isScrollbarVisible());
  EXPECT_EQ(view->getScrollbar().getThickness(), 6.0f);

  // Hidden state
  events.mouseMove({glm::vec2{195.0f, 20.0f}});
  events.mouseDown({glm::vec2{195.0f, 20.0f}});
  events.mouseMove({glm::vec2{1000.0, 1000.0f}});
  events.mouseUp({glm::vec2{1000.0f, 1000.0f}});
  EXPECT_FALSE(view->isScrollbarVisible());
}
