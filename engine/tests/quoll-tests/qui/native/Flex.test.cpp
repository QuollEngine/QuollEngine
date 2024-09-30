#include "quoll/core/Base.h"
#include "quoll/qui/native/Flex.h"
#include "quoll/qui/reactive/Scope.h"
#include "quoll-tests/Testing.h"
#include "MockComponent.h"

class QuiFlexTest : public ::testing::Test {
public:
};

TEST_F(QuiFlexTest, CreatesFlexWithElements) {
  qui::Element el = qui::Flex({MockComponent(10), MockComponent(20)});

  auto *flex = static_cast<const qui::Flex *>(el.getComponent());

  EXPECT_EQ(flex->getChildren().size(), 2);

  EXPECT_EQ(flex->getDirection(), qui::Direction::Row);
  EXPECT_EQ(flex->getShrink(), 1.0f);
  EXPECT_EQ(flex->getGrow(), 0.0f);
  EXPECT_EQ(flex->getSpacing(), glm::vec2(0.0f));
  EXPECT_EQ(flex->getWrap(), qui::Wrap::NoWrap);

  auto *child1 = static_cast<const MockComponent *>(
      flex->getChildren().at(0).getComponent());
  EXPECT_EQ(child1->value, 10);

  auto *child2 = static_cast<const MockComponent *>(
      flex->getChildren().at(1).getComponent());
  EXPECT_EQ(child2->value, 20);

  auto *view = static_cast<qui::FlexView *>(el.getView());
  EXPECT_EQ(view->getChildren().size(), 0);
  EXPECT_EQ(view->getDirection(), qui::Direction::Row);
  EXPECT_EQ(view->getShrink(), 1.0f);
  EXPECT_EQ(view->getGrow(), 0.0f);
  EXPECT_EQ(view->getSpacing(), glm::vec2(0.0f));
  EXPECT_EQ(view->getWrap(), qui::Wrap::NoWrap);
}

TEST_F(QuiFlexTest, CreatesFlexWithAllProps) {
  qui::Element el = qui::Flex({MockComponent(10), MockComponent(20)})
                        .direction(qui::Direction::Column)
                        .wrap(qui::Wrap::Wrap)
                        .shrink(2.0f)
                        .grow(3.0f)
                        .spacing(glm::vec2{2.0f, 5.0f});

  auto *flex = static_cast<const qui::Flex *>(el.getComponent());

  EXPECT_EQ(flex->getDirection(), qui::Direction::Column);
  EXPECT_EQ(flex->getShrink(), 2.0f);
  EXPECT_EQ(flex->getGrow(), 3.0f);
  EXPECT_EQ(flex->getSpacing(), glm::vec2(2.0f, 5.0f));
  EXPECT_EQ(flex->getWrap(), qui::Wrap::Wrap);

  auto *child1 = static_cast<const MockComponent *>(
      flex->getChildren().at(0).getComponent());
  EXPECT_EQ(child1->value, 10);

  auto *child2 = static_cast<const MockComponent *>(
      flex->getChildren().at(1).getComponent());
  EXPECT_EQ(child2->value, 20);

  auto *view = static_cast<qui::FlexView *>(el.getView());
  EXPECT_EQ(view->getChildren().size(), 0);
  EXPECT_EQ(view->getDirection(), qui::Direction::Row);
  EXPECT_EQ(view->getShrink(), 1.0f);
  EXPECT_EQ(view->getGrow(), 0.0f);
  EXPECT_EQ(view->getSpacing(), glm::vec2(0.0f));
  EXPECT_EQ(view->getWrap(), qui::Wrap::NoWrap);
}

TEST_F(QuiFlexTest, BuildingFlexUpdatesView) {
  qui::Element el = qui::Flex({MockComponent(10), MockComponent(20)})
                        .direction(qui::Direction::Column)
                        .wrap(qui::Wrap::Wrap)
                        .shrink(2.0f)
                        .grow(3.0f)
                        .spacing(glm::vec2{2.0f, 5.0f});

  el.build();
  auto *view = static_cast<qui::FlexView *>(el.getView());

  EXPECT_EQ(view->getChildren().size(), 2);
  EXPECT_EQ(view->getDirection(), qui::Direction::Column);
  EXPECT_EQ(view->getWrap(), qui::Wrap::Wrap);
  EXPECT_EQ(view->getShrink(), 2.0f);
  EXPECT_EQ(view->getGrow(), 3.0f);
  EXPECT_EQ(view->getSpacing(), glm::vec2(2.0f, 5.0f));

  auto *child1 = static_cast<MockView *>(view->getChildren().at(0));
  EXPECT_EQ(child1->value, 10);

  auto *child2 = static_cast<MockView *>(view->getChildren().at(1));
  EXPECT_EQ(child2->value, 20);
}

TEST_F(QuiFlexTest, UpdatingFlexPropertiesAfterBuildUpdatesTheView) {
  qui::Scope scope;
  auto children =
      scope.signal<qui::Element>({MockComponent(10), MockComponent(20)});
  auto direction = scope.signal(qui::Direction::Column);
  auto wrap = scope.signal(qui::Wrap::Wrap);
  auto shrink = scope.signal(2.0f);
  auto grow = scope.signal(3.0f);
  auto spacing = scope.signal(glm::vec2{2.0f, 5.0f});

  qui::Element el = qui::Flex(children)
                        .direction(direction)
                        .wrap(wrap)
                        .shrink(shrink)
                        .grow(grow)
                        .spacing(spacing);

  el.build();

  auto *view = static_cast<qui::FlexView *>(el.getView());

  {
    EXPECT_EQ(view->getChildren().size(), 2);
    EXPECT_EQ(view->getDirection(), qui::Direction::Column);
    EXPECT_EQ(view->getWrap(), qui::Wrap::Wrap);
    EXPECT_EQ(view->getShrink(), 2.0f);
    EXPECT_EQ(view->getGrow(), 3.0f);
    EXPECT_EQ(view->getSpacing(), glm::vec2(2.0f, 5.0f));

    auto *child1 = static_cast<MockView *>(view->getChildren().at(0));
    EXPECT_EQ(child1->value, 10);

    auto *child2 = static_cast<MockView *>(view->getChildren().at(1));
    EXPECT_EQ(child2->value, 20);
  }

  children.push_back(MockComponent(30));
  {
    EXPECT_EQ(view->getChildren().size(), 3);

    auto *child1 = static_cast<MockView *>(view->getChildren().at(0));
    EXPECT_EQ(child1->value, 10);

    auto *child2 = static_cast<MockView *>(view->getChildren().at(1));
    EXPECT_EQ(child2->value, 20);

    auto *child3 = static_cast<MockView *>(view->getChildren().at(2));
    EXPECT_EQ(child3->value, 30);
  }

  direction.set(qui::Direction::Row);
  EXPECT_EQ(view->getDirection(), qui::Direction::Row);

  wrap.set(qui::Wrap::NoWrap);
  EXPECT_EQ(view->getWrap(), qui::Wrap::NoWrap);

  shrink.set(3.0f);
  EXPECT_EQ(view->getShrink(), 3.0f);

  grow.set(4.0f);
  EXPECT_EQ(view->getGrow(), 4.0f);

  spacing.set(glm::vec2{5.0f, 10.0f});
  EXPECT_EQ(view->getSpacing(), glm::vec2(5.0f, 10.0f));
}
