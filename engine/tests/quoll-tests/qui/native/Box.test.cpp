#include "quoll/core/Base.h"
#include "quoll/qui/native/Box.h"
#include "quoll/qui/reactive/Scope.h"
#include "MockComponent.h"
#include "QuiComponentTest.h"

class QuiBoxTest : public QuiComponentTest {
public:
};

TEST_F(QuiBoxTest, CreatesBoxWithElements) {
  qui::Element el = qui::Box(MockComponent(10));

  auto *box = static_cast<const qui::Box *>(el.getComponent());

  auto *test1 =
      static_cast<const MockComponent *>(box->getChild().getComponent());
  EXPECT_EQ(test1->value, 10);

  EXPECT_EQ(box->getPadding(), qui::EdgeInsets(0.0f));
  EXPECT_EQ(box->getBackground(), qui::Color::Transparent);
  EXPECT_EQ(box->getWidth(), 0.0f);
  EXPECT_EQ(box->getHeight(), 0.0f);
  EXPECT_EQ(box->getBorderRadius(), 0.0f);

  auto *view = static_cast<qui::BoxView *>(el.getView());
  EXPECT_EQ(view->getChild(), nullptr);
  EXPECT_EQ(view->getPadding(), qui::EdgeInsets(0.0f));
  EXPECT_EQ(view->getBackground(), qui::Color::Transparent);
  EXPECT_EQ(view->getWidth(), 0.0f);
  EXPECT_EQ(view->getHeight(), 0.0f);
}

TEST_F(QuiBoxTest, CreatesBoxWithAllProps) {
  qui::Element el = qui::Box(MockComponent(10))
                        .background(qui::Color::Red)
                        .padding(qui::EdgeInsets(3.5f))
                        .borderRadius(5.0f)
                        .width(10.0f)
                        .height(20.0f);

  auto *box = static_cast<const qui::Box *>(el.getComponent());

  auto *test1 =
      static_cast<const MockComponent *>(box->getChild().getComponent());
  EXPECT_EQ(test1->value, 10);

  EXPECT_EQ(box->getPadding(), qui::EdgeInsets(3.5f));
  EXPECT_EQ(box->getBackground(), qui::Color::Red);
  EXPECT_EQ(box->getWidth(), 10.0f);
  EXPECT_EQ(box->getHeight(), 20.0f);
  EXPECT_EQ(box->getBorderRadius(), 5.0f);

  auto *view = static_cast<qui::BoxView *>(el.getView());
  EXPECT_EQ(view->getChild(), nullptr);
  EXPECT_EQ(view->getPadding(), qui::EdgeInsets(0.0f));
  EXPECT_EQ(view->getBackground(), qui::Color::Transparent);
  EXPECT_EQ(view->getWidth(), 0.0f);
  EXPECT_EQ(view->getHeight(), 0.0f);
}

TEST_F(QuiBoxTest, BuildingBoxUpdatesView) {
  qui::Element el = qui::Box(MockComponent(10))
                        .background(qui::Color::Red)
                        .padding(qui::EdgeInsets(3.5f))
                        .borderRadius(5.0f)
                        .width(10.0f)
                        .height(20.0f);

  el.build(buildContext);

  auto *view = static_cast<qui::BoxView *>(el.getView());

  auto *childView = static_cast<MockView *>(view->getChild());
  EXPECT_EQ(childView->value, 10);

  EXPECT_EQ(view->getPadding(), qui::EdgeInsets(3.5f));
  EXPECT_EQ(view->getBackground(), qui::Color::Red);
  EXPECT_EQ(view->getWidth(), 10.0f);
  EXPECT_EQ(view->getHeight(), 20.0f);
  EXPECT_EQ(view->getBorderRadius(), 5.0f);
}

TEST_F(QuiBoxTest, UpdatingBoxPropertiesAfterBuildUpdatesTheView) {
  qui::Scope scope;
  auto padding = scope.signal(qui::EdgeInsets(3.5));
  auto background = scope.signal(qui::Color::Red);
  auto borderRadius = scope.signal(5.0f);
  auto width = scope.signal(10.0f);
  auto height = scope.signal(20.0f);
  auto child = scope.signal<qui::Element>(MockComponent(10));

  qui::Element el = qui::Box(child)
                        .background(background)
                        .padding(padding)
                        .borderRadius(borderRadius)
                        .width(width)
                        .height(height);

  auto *box = static_cast<const qui::Box *>(el.getComponent());
  el.build(buildContext);

  auto *view = static_cast<qui::BoxView *>(el.getView());

  {
    auto *childView = static_cast<MockView *>(view->getChild());
    EXPECT_EQ(childView->value, 10);

    EXPECT_EQ(view->getPadding(), qui::EdgeInsets(3.5f));
    EXPECT_EQ(view->getBackground(), qui::Color::Red);
    EXPECT_EQ(view->getWidth(), 10.0f);
    EXPECT_EQ(view->getHeight(), 20.0f);
    EXPECT_EQ(view->getBorderRadius(), 5.0f);
  }

  child.set(MockComponent(20));
  auto *childView = static_cast<MockView *>(view->getChild());
  EXPECT_EQ(childView->value, 20);

  padding.set(qui::EdgeInsets(5.0f));
  EXPECT_EQ(view->getPadding(), qui::EdgeInsets(5.0f));

  background.set(qui::Color::Blue);
  EXPECT_EQ(view->getBackground(), qui::Color::Blue);

  borderRadius.set(10.0f);
  EXPECT_EQ(view->getBorderRadius(), 10.0f);

  width.set(20.0f);
  EXPECT_EQ(view->getWidth(), 20.0f);

  height.set(30.0f);
  EXPECT_EQ(view->getHeight(), 30.0f);
}
