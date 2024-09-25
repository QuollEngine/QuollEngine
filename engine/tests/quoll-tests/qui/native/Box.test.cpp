#include "quoll/core/Base.h"
#include "quoll/qui/native/Box.h"
#include "quoll/qui/reactive/Scope.h"
#include "quoll-tests/Testing.h"

class TestComponentView : public qui::View {
public:
  u32 value = 0;

  qui::LayoutOutput layout(const qui::LayoutInput &input) override {
    return qui::LayoutOutput{};
  }
};

class TestComponent : public qui::Component {
public:
  TestComponent(u32 value) : value(value) {}

  void build() override { mView.value = value; }

  qui::View *getView() override { return &mView; }

  u32 value;
  TestComponentView mView;
};

class QuiBoxTest : public ::testing::Test {
public:
};

TEST_F(QuiBoxTest, CreatesBoxWithElements) {
  qui::Element el = qui::Box(qui::Element(TestComponent(10)));

  auto *box = static_cast<qui::Box *>(el.getComponent());

  auto *test1 = static_cast<TestComponent *>(box->getChild().getComponent());
  EXPECT_EQ(test1->value, 10);

  EXPECT_EQ(box->getPadding(), qui::EdgeInsets(0.0f));
  EXPECT_EQ(box->getBackground(), qui::Color::Black);
  EXPECT_EQ(box->getWidth(), 0.0f);
  EXPECT_EQ(box->getHeight(), 0.0f);
  EXPECT_EQ(box->getBorderRadius(), 0.0f);
}

TEST_F(QuiBoxTest, CreatesBoxWithAllProps) {
  qui::Element el = qui::Box(qui::Element(TestComponent(10)))
                        .background(qui::Color::Red)
                        .padding(qui::EdgeInsets(3.5f))
                        .borderRadius(5.0f)
                        .width(10.0f)
                        .height(20.0f);

  auto *box = static_cast<qui::Box *>(el.getComponent());

  auto *test1 = static_cast<TestComponent *>(box->getChild().getComponent());
  EXPECT_EQ(test1->value, 10);

  EXPECT_EQ(box->getPadding(), qui::EdgeInsets(3.5f));
  EXPECT_EQ(box->getBackground(), qui::Color::Red);
  EXPECT_EQ(box->getWidth(), 10.0f);
  EXPECT_EQ(box->getHeight(), 20.0f);
  EXPECT_EQ(box->getBorderRadius(), 5.0f);

  auto *view = static_cast<qui::BoxView *>(box->getView());
  EXPECT_EQ(view->getChild(), nullptr);
  EXPECT_EQ(view->getPadding(), qui::EdgeInsets(0.0f));
  EXPECT_EQ(view->getBackground(), qui::Color::Black);
  EXPECT_EQ(view->getWidth(), 0.0f);
  EXPECT_EQ(view->getHeight(), 0.0f);
}

TEST_F(QuiBoxTest, BuildingBoxUpdatesView) {
  qui::Element el = qui::Box(qui::Element(TestComponent(10)))
                        .background(qui::Color::Red)
                        .padding(qui::EdgeInsets(3.5f))
                        .borderRadius(5.0f)
                        .width(10.0f)
                        .height(20.0f);

  auto *box = static_cast<qui::Box *>(el.getComponent());
  box->build();

  auto *view = static_cast<qui::BoxView *>(box->getView());

  auto *childView = static_cast<TestComponentView *>(view->getChild());
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
  auto child = scope.signal(qui::Element(TestComponent(10)));

  qui::Element el = qui::Box(child)
                        .background(background)
                        .padding(padding)
                        .borderRadius(borderRadius)
                        .width(width)
                        .height(height);

  auto *box = static_cast<qui::Box *>(el.getComponent());
  box->build();

  auto *view = static_cast<qui::BoxView *>(box->getView());

  {
    auto *childView = static_cast<TestComponentView *>(view->getChild());
    EXPECT_EQ(childView->value, 10);

    EXPECT_EQ(view->getPadding(), qui::EdgeInsets(3.5f));
    EXPECT_EQ(view->getBackground(), qui::Color::Red);
    EXPECT_EQ(view->getWidth(), 10.0f);
    EXPECT_EQ(view->getHeight(), 20.0f);
    EXPECT_EQ(view->getBorderRadius(), 5.0f);
  }

  child.set(TestComponent(20));
  auto *childView = static_cast<TestComponentView *>(view->getChild());
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
