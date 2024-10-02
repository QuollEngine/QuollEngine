#include "quoll/core/Base.h"
#include "quoll/qui/native/Pressable.h"
#include "quoll/qui/reactive/Scope.h"
#include "MockComponent.h"
#include "QuiComponentTest.h"

class QuiPressableTest : public QuiComponentTest {
public:
  using PressHandler = ::testing::MockFunction<void(const qui::PressEvent &)>;
};

TEST_F(QuiPressableTest, CreatesPressableElementWithChild) {
  qui::Element element = qui::Pressable(MockComponent(20));

  auto *component = static_cast<const qui::Pressable *>(element.getComponent());

  EXPECT_FALSE(component->getOnPress());
  EXPECT_FALSE(component->getOnPressDown());
  EXPECT_FALSE(component->getOnPressUp());
  EXPECT_FALSE(component->getOnHoverIn());
  EXPECT_FALSE(component->getOnHoverOut());

  auto *child =
      dynamic_cast<const MockComponent *>(component->getChild().getComponent());
  EXPECT_EQ(child->value, 20);

  auto *view = dynamic_cast<MockView *>(element.getView());
  ASSERT_NE(view, nullptr);

  EXPECT_EQ(view->value, 0);
  EXPECT_EQ(child->mView.value, 0);
}

TEST_F(QuiPressableTest, CreatesPressableElementWithAllProps) {
  PressHandler handlePress, handlePressDown, handlePressUp, handleHoverIn,
      handleHoverOut;

  EXPECT_CALL(handlePress, Call(::testing::_)).Times(1);
  EXPECT_CALL(handlePressDown, Call(::testing::_)).Times(1);
  EXPECT_CALL(handlePressUp, Call(::testing::_)).Times(1);
  EXPECT_CALL(handleHoverIn, Call(::testing::_)).Times(1);
  EXPECT_CALL(handleHoverOut, Call(::testing::_)).Times(1);

  qui::Element element = qui::Pressable(MockComponent(20))
                             .onPress(handlePress.AsStdFunction())
                             .onPressDown(handlePressDown.AsStdFunction())
                             .onPressUp(handlePressUp.AsStdFunction())
                             .onHoverIn(handleHoverIn.AsStdFunction())
                             .onHoverOut(handleHoverOut.AsStdFunction());

  auto *component = static_cast<const qui::Pressable *>(element.getComponent());

  EXPECT_TRUE(component->getOnPress());
  EXPECT_TRUE(component->getOnPressDown());
  EXPECT_TRUE(component->getOnPressUp());
  EXPECT_TRUE(component->getOnHoverIn());
  EXPECT_TRUE(component->getOnHoverOut());

  auto *child =
      static_cast<const MockComponent *>(component->getChild().getComponent());
  EXPECT_EQ(child->value, 20);

  auto *view = dynamic_cast<MockView *>(element.getView());
  ASSERT_NE(view, nullptr);

  EXPECT_EQ(view->value, 0);
  EXPECT_EQ(child->mView.value, 0);

  qui::PressEvent ev{};
  component->getOnPress()(ev);
  component->getOnPressDown()(ev);
  component->getOnPressUp()(ev);
  component->getOnHoverIn()(ev);
  component->getOnHoverOut()(ev);
}

TEST_F(QuiPressableTest, BuildCallsChildBuild) {
  qui::Element element = qui::Pressable(MockComponent(20));

  element.build(buildContext);

  auto *component = static_cast<const qui::Pressable *>(element.getComponent());
  auto *child =
      static_cast<const MockComponent *>(component->getChild().getComponent());
  EXPECT_EQ(child->value, 20);

  auto *view = dynamic_cast<MockView *>(element.getView());
  ASSERT_NE(view, nullptr);
  EXPECT_EQ(view->value, 20);
  EXPECT_EQ(child->mView.value, 20);
}

TEST_F(QuiPressableTest, UpdatingChildRebuildsChild) {
  PressHandler handlePress, handlePressDown, handlePressUp, handleHoverIn,
      handleHoverOut;

  qui::Scope scope;
  auto childEl = scope.signal<qui::Element>(MockComponent(20));

  qui::Element element = qui::Pressable(childEl);

  element.build(buildContext);

  {
    auto *component =
        static_cast<const qui::Pressable *>(element.getComponent());
    auto *child = static_cast<const MockComponent *>(
        component->getChild().getComponent());
    EXPECT_EQ(child->value, 20);

    auto *view = dynamic_cast<MockView *>(element.getView());
    ASSERT_NE(view, nullptr);
    EXPECT_EQ(view->value, 20);
    EXPECT_EQ(child->mView.value, 20);
  }

  childEl.set(MockComponent(40));
  {
    auto *component =
        static_cast<const qui::Pressable *>(element.getComponent());
    auto *child = static_cast<const MockComponent *>(
        component->getChild().getComponent());
    EXPECT_EQ(child->value, 40);

    auto *view = dynamic_cast<MockView *>(element.getView());
    ASSERT_NE(view, nullptr);
    EXPECT_EQ(view->value, 40);
    EXPECT_EQ(child->mView.value, 40);
  }
}

using QuiPressableEventsTest = QuiPressableTest;

MATCHER_P(PressEventEq, point, "") { return arg.point == point; }

TEST_F(QuiPressableEventsTest,
       MouseClickEventCallsPressHandlerIfViewHitTestIsTrueForMousePosition) {
  PressHandler pressHandler;

  EXPECT_CALL(pressHandler, Call(PressEventEq(glm::vec2{50.0f, 60.0f})))
      .Times(1);

  MockComponent component(20);
  component.mView.desiredSize = {100, 100};

  qui::Element element =
      qui::Pressable(component).onPress(pressHandler.AsStdFunction());

  element.build(buildContext);
  element.getView()->layout({});

  ASSERT_EQ(buildContext.eventManager->getMouseClickHandlers().size(), 1);
  auto mouseHandler = buildContext.eventManager->getMouseClickHandlers().at(0);
  mouseHandler(qui::MouseEvent{glm::vec2{50, 60}});
  mouseHandler(qui::MouseEvent{glm::vec2{150, 120}});
}

TEST_F(QuiPressableEventsTest,
       MouseDownEventCallsPressDownHandlerIfViewHitTestIsTrueForMousePosition) {
  PressHandler pressHandler;

  EXPECT_CALL(pressHandler, Call(PressEventEq(glm::vec2{50.0f, 60.0f})))
      .Times(1);

  MockComponent component(20);
  component.mView.desiredSize = {100, 100};

  qui::Element element =
      qui::Pressable(component).onPressDown(pressHandler.AsStdFunction());

  element.build(buildContext);
  element.getView()->layout({});

  ASSERT_EQ(buildContext.eventManager->getMouseDownHandlers().size(), 1);
  auto mouseHandler = buildContext.eventManager->getMouseDownHandlers().at(0);
  mouseHandler(qui::MouseEvent{glm::vec2{50, 60}});
  mouseHandler(qui::MouseEvent{glm::vec2{150, 120}});
}

TEST_F(QuiPressableEventsTest,
       MouseUpEventCallsPressUpHandlerIfViewHitTestIsTrueForMousePosition) {
  PressHandler pressHandler;

  EXPECT_CALL(pressHandler, Call(PressEventEq(glm::vec2{50.0f, 60.0f})))
      .Times(1);

  MockComponent component(20);
  component.mView.desiredSize = {100, 100};

  qui::Element element =
      qui::Pressable(component).onPressDown(pressHandler.AsStdFunction());

  element.build(buildContext);
  element.getView()->layout({});

  ASSERT_EQ(buildContext.eventManager->getMouseDownHandlers().size(), 1);

  auto mouseHandler = buildContext.eventManager->getMouseDownHandlers().at(0);
  mouseHandler(qui::MouseEvent{glm::vec2{50, 60}});
  mouseHandler(qui::MouseEvent{glm::vec2{150, 120}});
}

TEST_F(
    QuiPressableEventsTest,
    MouseMoveEventCallsHoverInHandlerIfViewHitTestIsTrueForMousePositionAndElementIsNotHoveredYet) {
  PressHandler pressHandler;

  EXPECT_CALL(pressHandler, Call(PressEventEq(glm::vec2{50.0f, 60.0f})))
      .Times(1);

  MockComponent component(20);
  component.mView.desiredSize = {100, 100};

  qui::Element element =
      qui::Pressable(component).onHoverIn(pressHandler.AsStdFunction());

  element.build(buildContext);
  element.getView()->layout({});

  ASSERT_EQ(buildContext.eventManager->getMouseMoveHandlers().size(), 1);

  auto mouseHandler = buildContext.eventManager->getMouseMoveHandlers().at(0);
  mouseHandler(qui::MouseEvent{glm::vec2{50, 60}});
  mouseHandler(qui::MouseEvent{glm::vec2{60, 70}});
  mouseHandler(qui::MouseEvent{glm::vec2{150, 120}});
}

TEST_F(
    QuiPressableEventsTest,
    MouseMoveEventCallsHoverOutHandlerIfViewHitTestIsTrueForMousePositionAndElementIsAlreadyHovered) {
  PressHandler pressHandler;

  EXPECT_CALL(pressHandler, Call(PressEventEq(glm::vec2{150.0f, 120.0f})))
      .Times(1);

  MockComponent component(20);
  component.mView.desiredSize = {100, 100};

  qui::Element element =
      qui::Pressable(component).onHoverOut(pressHandler.AsStdFunction());

  element.build(buildContext);
  element.getView()->layout({});

  ASSERT_EQ(buildContext.eventManager->getMouseMoveHandlers().size(), 1);

  auto mouseHandler = buildContext.eventManager->getMouseMoveHandlers().at(0);
  mouseHandler(qui::MouseEvent{glm::vec2{50, 60}});
  mouseHandler(qui::MouseEvent{glm::vec2{60, 70}});
  mouseHandler(qui::MouseEvent{glm::vec2{150, 120}});
  mouseHandler(qui::MouseEvent{glm::vec2{250, 320}});
}

TEST_F(
    QuiPressableEventsTest,
    MouseMoveEventsCallBothHoverInAndHoverOutHandlersIfMousePositionGoesInAndOutOfView) {
  PressHandler hoverInHandler, hoverOutHandler;

  EXPECT_CALL(hoverInHandler, Call(PressEventEq(glm::vec2{50.0f, 60.0f})))
      .Times(1);
  EXPECT_CALL(hoverOutHandler, Call(PressEventEq(glm::vec2{150.0f, 120.0f})))
      .Times(1);

  MockComponent component(20);
  component.mView.desiredSize = {100, 100};

  qui::Element element = qui::Pressable(component)
                             .onHoverIn(hoverInHandler.AsStdFunction())
                             .onHoverOut(hoverOutHandler.AsStdFunction());

  element.build(buildContext);
  element.getView()->layout({});

  ASSERT_EQ(buildContext.eventManager->getMouseMoveHandlers().size(), 1);

  auto mouseHandler = buildContext.eventManager->getMouseMoveHandlers().at(0);
  mouseHandler(qui::MouseEvent{glm::vec2{50, 60}});
  mouseHandler(qui::MouseEvent{glm::vec2{60, 70}});
  mouseHandler(qui::MouseEvent{glm::vec2{150, 120}});
  mouseHandler(qui::MouseEvent{glm::vec2{250, 320}});
}

TEST_F(QuiPressableEventsTest,
       EventsAreClearedIsComponentIsDestroyedAfterBuild) {

  {
    PressHandler handlePress, handlePressDown, handlePressUp, handleHoverIn,
        handleHoverOut;

    MockComponent component(20);
    component.mView.desiredSize = {100, 100};

    qui::Element element = qui::Pressable(component)
                               .onPress(handlePress.AsStdFunction())
                               .onPressDown(handlePressDown.AsStdFunction())
                               .onPressUp(handlePressUp.AsStdFunction())
                               .onHoverIn(handleHoverIn.AsStdFunction())
                               .onHoverOut(handleHoverOut.AsStdFunction());

    element.build(buildContext);

    EXPECT_EQ(buildContext.eventManager->getMouseClickHandlers().size(), 1);
    EXPECT_EQ(buildContext.eventManager->getMouseDownHandlers().size(), 1);
    EXPECT_EQ(buildContext.eventManager->getMouseUpHandlers().size(), 1);
    EXPECT_EQ(buildContext.eventManager->getMouseMoveHandlers().size(), 1);
  }

  EXPECT_EQ(buildContext.eventManager->getMouseClickHandlers().size(), 0);
  EXPECT_EQ(buildContext.eventManager->getMouseDownHandlers().size(), 0);
  EXPECT_EQ(buildContext.eventManager->getMouseUpHandlers().size(), 0);
  EXPECT_EQ(buildContext.eventManager->getMouseMoveHandlers().size(), 0);
}
