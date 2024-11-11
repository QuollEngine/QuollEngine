#include "quoll/core/Base.h"
#include "quoll/qui/native/FlexView.h"
#include "quoll-tests/Testing.h"
#include "MockView.h"

class LayoutTest : public ::testing::TestWithParam<std::tuple<qui::Direction>> {
public:
  LayoutTest() : direction(std::get<0>(GetParam())) {
    mainAxis = direction == qui::Direction::Row ? 0 : 1;
    crossAxis = direction == qui::Direction::Row ? 1 : 0;

    view.setDirection(direction);
    view.setChildren({&child1, &child2, &child3});
  }

public:
  qui::Direction direction;
  glm::length_t mainAxis;
  glm::length_t crossAxis;

  MockView child1, child2, child3;
  qui::FlexView view;
};

TEST_P(LayoutTest, SetsViewPositionToInputPosition) {
  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({.position = position});

  EXPECT_EQ(view.getPosition(), position);
}

TEST_P(LayoutTest, SetsTotalSizeOfChildrenAsViewSizeIfSizeIsWithinConstraints) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 30.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 300.0f;
  child3.desiredSize[crossAxis] = 40.0f;

  qui::Constraints constraints;
  constraints.min[mainAxis] = 0.0f;
  constraints.max[mainAxis] = 1000.0f;
  constraints.min[crossAxis] = 0.0f;
  constraints.max[crossAxis] = 1000.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 450.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 40.0f);
}

TEST_P(LayoutTest, ConstraintsViewSizeToInputConstraints) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 30.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 300.0f;
  child3.desiredSize[crossAxis] = 40.0f;

  qui::Constraints constraints;
  constraints.min[mainAxis] = 0.0f;
  constraints.max[mainAxis] = 350.0f;
  constraints.min[crossAxis] = 150.0f;
  constraints.max[crossAxis] = 300.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 350.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 150.0f);
}

TEST_P(LayoutTest, SetsViewSizeToChildSizeIfConstraintsAreInfinite) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 30.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 300.0f;
  child3.desiredSize[crossAxis] = 40.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({qui::Constraints(), position});

  EXPECT_EQ(view.getSize()[mainAxis], 450.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 40.0f);
}

TEST_P(
    LayoutTest,
    SetsViewSizeToTotalSizeOfChildrenWithWrapIfSizeIsSmallerThanConstraints) {
  view.setWrap(qui::Wrap::Wrap);
  child1.desiredSize[mainAxis] = 80.0f;
  child1.desiredSize[crossAxis] = 50.0f;
  child2.desiredSize[mainAxis] = 60.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 120.0f;
  child3.desiredSize[crossAxis] = 30.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 200.0f;

  view.layout({constraints});

  EXPECT_EQ(view.getSize()[mainAxis], 140.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 80.0f);
}

TEST_P(
    LayoutTest,
    ConstraintsViewSizeIfTotalSizeOfChildrenWithWrapIsLargerThanConstraints) {
  view.setWrap(qui::Wrap::Wrap);
  child1.desiredSize[mainAxis] = 80.0f;
  child1.desiredSize[crossAxis] = 50.0f;
  child2.desiredSize[mainAxis] = 60.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 120.0f;
  child3.desiredSize[crossAxis] = 30.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 50.0f;

  view.layout({constraints});

  EXPECT_EQ(view.getSize()[mainAxis], 140.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 50.0f);
}

TEST_P(LayoutTest, RetainsChildSizesIfTotalSizeOfChildrenIsWithinConstraints) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 100.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 350.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 50.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 300.0f;
  constraints.max[crossAxis] = 200.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 300.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 200.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child1.getSize()[mainAxis], 100.0f);
  EXPECT_EQ(child1.getSize()[crossAxis], 100.0f);

  EXPECT_EQ(child2.getPosition()[mainAxis], 300.0f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child2.getSize()[mainAxis], 50.0f);
  EXPECT_EQ(child2.getSize()[crossAxis], 350.0f);

  EXPECT_EQ(child3.getPosition()[mainAxis], 350.0f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child3.getSize()[mainAxis], 150.0f);
  EXPECT_EQ(child3.getSize()[crossAxis], 50.0f);
}

TEST_P(LayoutTest,
       RetainsChildSizesButClampsViewToConstraintsIfShrinkFactorIsZero) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 100.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 350.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 50.0f;

  qui::FlexView f1, f2, f3;
  f1.setChildren({&child1});
  f2.setChildren({&child2});
  f3.setChildren({&child3});

  view.setChildren({&f1, &f2, &f3});

  f1.setShrink(0);
  f2.setShrink(0);
  f3.setShrink(0);

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 200.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[crossAxis], 200.0f);
  EXPECT_EQ(view.getSize()[mainAxis], 200.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child1.getSize()[mainAxis], 100.0f);
  EXPECT_EQ(child1.getSize()[crossAxis], 100.0f);

  EXPECT_EQ(child2.getPosition()[mainAxis], 300.0f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child2.getSize()[mainAxis], 50.0f);
  EXPECT_EQ(child2.getSize()[crossAxis], 350.0f);

  EXPECT_EQ(child3.getPosition()[mainAxis], 350.0f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child3.getSize()[mainAxis], 150.0f);
  EXPECT_EQ(child3.getSize()[crossAxis], 50.0f);
}

TEST_P(
    LayoutTest,
    ShrinksAllChildrenEquallyByDefaultIfTotalSizeOfChildrenIsLargerThanMaxSize) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 100.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 350.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 50.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[crossAxis], 150.0f);
  EXPECT_EQ(view.getSize()[mainAxis], 200.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(child1.getSize()[mainAxis], 66.667f, 0.001f);
  EXPECT_EQ(child1.getSize()[crossAxis], 100.0f);

  EXPECT_NEAR(child2.getPosition()[mainAxis], 266.667f, 0.001f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(child2.getSize()[mainAxis], 33.333f, 0.001f);
  EXPECT_EQ(child2.getSize()[crossAxis], 350.0f);

  EXPECT_NEAR(child3.getPosition()[mainAxis], 300.0f, 0.001f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(child3.getSize()[mainAxis], 100.0f, 0.001f);
  EXPECT_EQ(child3.getSize()[crossAxis], 50.0f);
}

TEST_P(LayoutTest,
       ShrinksChildrenByShrinkFactorIfTotalSizeOfChildrenIsLargerThanMaxSize) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 100.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 350.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 50.0f;

  qui::FlexView f1, f2, f3;
  f1.setChildren({&child1});
  f2.setChildren({&child2});
  f3.setChildren({&child3});

  view.setChildren({&f1, &f2, &f3});

  f2.setShrink(2);

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[crossAxis], 150.0f);
  EXPECT_EQ(view.getSize()[mainAxis], 200.0f);

  EXPECT_EQ(f1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(f1.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(f1.getSize()[mainAxis], 71.429f, 0.001f);
  EXPECT_EQ(f1.getSize()[crossAxis], 100.0f);

  EXPECT_NEAR(f2.getPosition()[mainAxis], 271.429f, 0.001f);
  EXPECT_EQ(f2.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(f2.getSize()[mainAxis], 21.429f, 0.001f);
  EXPECT_EQ(f2.getSize()[crossAxis], 350.0f);

  EXPECT_NEAR(f3.getPosition()[mainAxis], 292.857f, 0.001f);
  EXPECT_EQ(f3.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(f3.getSize()[mainAxis], 107.143f, 0.001f);
  EXPECT_EQ(f3.getSize()[crossAxis], 50.0f);
}

TEST_P(LayoutTest, ShrinksChildrenIfHeightOfAChildIsLargerThanMaxSize) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 100.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 350.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 50.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 100.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[mainAxis] = 100.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[crossAxis], 150.0f);
  EXPECT_EQ(view.getSize()[mainAxis], 100.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 100.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(child1.getSize()[mainAxis], 33.333f, 0.001f);
  EXPECT_EQ(child1.getSize()[crossAxis], 100.0f);

  EXPECT_NEAR(child2.getPosition()[mainAxis], 133.333f, 0.001f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(child2.getSize()[mainAxis], 16.667f, 0.001f);
  EXPECT_EQ(child2.getSize()[crossAxis], 350.0f);

  EXPECT_NEAR(child3.getPosition()[mainAxis], 150.0f, 0.001f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(child3.getSize()[mainAxis], 50.0f, 0.001f);
  EXPECT_EQ(child3.getSize()[crossAxis], 50.0f);
}

TEST_P(
    LayoutTest,
    ShrinksChildrenWithSpacingAppliedIfTotalSizeOfChildrenIsLargerThanMaxSizeWithSpacing) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 100.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 350.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 50.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  glm::vec2 spacing;
  spacing[mainAxis] = 5.0f;
  spacing[crossAxis] = 0.0f;
  view.setSpacing(spacing);
  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[crossAxis], 150.0f);
  EXPECT_EQ(view.getSize()[mainAxis], 200.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(child1.getSize()[mainAxis], 63.333f, 0.001f);
  EXPECT_EQ(child1.getSize()[crossAxis], 100.0f);

  EXPECT_NEAR(child2.getPosition()[mainAxis], 268.333f, 0.001f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(child2.getSize()[mainAxis], 31.667f, 0.001f);
  EXPECT_EQ(child2.getSize()[crossAxis], 350.0f);

  EXPECT_EQ(child3.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(child3.getPosition()[mainAxis], 305.0f, 0.001f);
  EXPECT_EQ(child3.getSize()[crossAxis], 50.0f);
  EXPECT_NEAR(child3.getSize()[mainAxis], 95.0f, 0.001f);
}

TEST_P(LayoutTest,
       GrowsAllChildrenByGrowFactorIfTotalSizeOfChildrenIsSmallerThanMaxSize) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 100.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 350.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 50.0f;

  qui::FlexView f1, f2, f3;
  f1.setChildren({&child1});
  f2.setChildren({&child2});
  f3.setChildren({&child3});
  view.setChildren({&f1, &f2, &f3});

  f1.setGrow(1);
  f2.setGrow(2);
  f3.setGrow(1);

  qui::Constraints constraints;
  constraints.max[mainAxis] = 500.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[crossAxis], 150.0f);
  EXPECT_EQ(view.getSize()[mainAxis], 500.0f);

  EXPECT_EQ(f1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(f1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(f1.getSize()[mainAxis], 150.0f);
  EXPECT_EQ(f1.getSize()[crossAxis], 100.0f);

  EXPECT_EQ(f2.getPosition()[mainAxis], 350.0f);
  EXPECT_EQ(f2.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(f2.getSize()[mainAxis], 150.0f);
  EXPECT_EQ(f2.getSize()[crossAxis], 350.0f);

  EXPECT_EQ(f3.getPosition()[mainAxis], 500.0f);
  EXPECT_EQ(f3.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(f3.getSize()[mainAxis], 200.0f);
  EXPECT_EQ(f3.getSize()[crossAxis], 50.0f);
}

TEST_P(
    LayoutTest,
    GrowsChildrenWithSpacingAppliedIfTotalSizeOfChildrenIsSmallerThanMaxSizeWithSpacing) {
  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 100.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 350.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 50.0f;

  qui::FlexView f1, f2, f3;
  f1.setChildren({&child1});
  f2.setChildren({&child2});
  f3.setChildren({&child3});
  view.setChildren({&f1, &f2, &f3});

  f1.setGrow(1);
  f2.setGrow(1);
  f3.setGrow(1);

  qui::Constraints constraints;
  constraints.max[mainAxis] = 500.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  glm::vec2 spacing;
  spacing[mainAxis] = 5.0f;
  spacing[crossAxis] = 0.0f;
  view.setSpacing(spacing);
  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[crossAxis], 150.0f);
  EXPECT_EQ(view.getSize()[mainAxis], 500.0f);

  EXPECT_EQ(f1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(f1.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(f1.getSize()[mainAxis], 163.333f, 0.001f);
  EXPECT_EQ(f1.getSize()[crossAxis], 100.0f);

  EXPECT_NEAR(f2.getPosition()[mainAxis], 368.333f, 0.001f);
  EXPECT_EQ(f2.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(f2.getSize()[mainAxis], 113.333f, 0.001f);
  EXPECT_EQ(f2.getSize()[crossAxis], 350.0f);

  EXPECT_NEAR(f3.getPosition()[mainAxis], 486.667f, 0.001f);
  EXPECT_EQ(f3.getPosition()[crossAxis], 150.0f);
  EXPECT_NEAR(f3.getSize()[mainAxis], 213.333f, 0.001f);
  EXPECT_EQ(f3.getSize()[crossAxis], 50.0f);
}

TEST_P(
    LayoutTest,
    KeepsChildrenInOneSingleFlexLineIfWrappingIsEnabledButAllItemsCanFitIntoMainAxis) {
  view.setWrap(qui::Wrap::Wrap);

  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 100.0f;
  child2.desiredSize[mainAxis] = 50.0f;
  child2.desiredSize[crossAxis] = 350.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 50.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 300.0f;
  constraints.max[crossAxis] = 200.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 300.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 200.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child1.getSize()[mainAxis], 100.0f);
  EXPECT_EQ(child1.getSize()[crossAxis], 100.0f);

  EXPECT_EQ(child2.getPosition()[mainAxis], 300.0f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child2.getSize()[mainAxis], 50.0f);
  EXPECT_EQ(child2.getSize()[crossAxis], 350.0f);

  EXPECT_EQ(child3.getPosition()[mainAxis], 350.0f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child3.getSize()[mainAxis], 150.0f);
  EXPECT_EQ(child3.getSize()[crossAxis], 50.0f);
}

TEST_P(
    LayoutTest,
    WrapsItemsAndKeepsSizesTheSameIfWrappingIsEnabledAndEachItemCanFitInOneLineButAllOfThemCannot) {
  view.setWrap(qui::Wrap::Wrap);

  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 50.0f;
  child2.desiredSize[mainAxis] = 140.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 120.0f;
  child3.desiredSize[crossAxis] = 30.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[crossAxis] = 150.0f;
  position[mainAxis] = 200.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 140.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 100.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child1.getSize()[mainAxis], 100.0f);
  EXPECT_EQ(child1.getSize()[crossAxis], 50.0f);

  EXPECT_EQ(child2.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 200.0f);
  EXPECT_EQ(child2.getSize()[mainAxis], 140.0f);
  EXPECT_EQ(child2.getSize()[crossAxis], 20.0f);

  EXPECT_EQ(child3.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 220.0f);
  EXPECT_EQ(child3.getSize()[mainAxis], 120.0f);
  EXPECT_EQ(child3.getSize()[crossAxis], 30.0f);
}

TEST_P(
    LayoutTest,
    WrapsItemsAndKeepsSizesTheSameIfWrappingIsEnabledAndItemCannotFitInOneLineButShrinkFactorIsZero) {
  view.setWrap(qui::Wrap::Wrap);

  child1.desiredSize[mainAxis] = 250.0f;
  child1.desiredSize[crossAxis] = 50.0f;
  child2.desiredSize[mainAxis] = 300.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 350.0f;
  child3.desiredSize[crossAxis] = 30.0f;

  qui::FlexView f1, f2, f3;
  f1.setChildren({&child1});
  f2.setChildren({&child2});
  f3.setChildren({&child3});

  view.setChildren({&f1, &f2, &f3});

  f1.setShrink(0);
  f2.setShrink(0);
  f3.setShrink(0);

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[crossAxis] = 150.0f;
  position[mainAxis] = 200.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 200.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 100.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child1.getSize()[mainAxis], 250.0f);
  EXPECT_EQ(child1.getSize()[crossAxis], 50.0f);

  EXPECT_EQ(child2.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 200.0f);
  EXPECT_EQ(child2.getSize()[mainAxis], 300.0f);
  EXPECT_EQ(child2.getSize()[crossAxis], 20.0f);

  EXPECT_EQ(child3.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 220.0f);
  EXPECT_EQ(child3.getSize()[mainAxis], 350.0f);
  EXPECT_EQ(child3.getSize()[crossAxis], 30.0f);
}

TEST_P(
    LayoutTest,
    WrapsItemsAndShrinksSizesToFitWithinLineIfWrappingIsEnabledAndSingleItemIsBiggerThanFlexLine) {
  view.setWrap(qui::Wrap::Wrap);

  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 50.0f;
  child2.desiredSize[mainAxis] = 250.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 200.0f;
  child3.desiredSize[crossAxis] = 30.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 200.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 100.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child1.getSize()[mainAxis], 100.0f);
  EXPECT_EQ(child1.getSize()[crossAxis], 50.0f);

  EXPECT_EQ(child2.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 200.0f);
  EXPECT_EQ(child2.getSize()[mainAxis], 200.0f);
  EXPECT_EQ(child2.getSize()[crossAxis], 20.0f);

  EXPECT_EQ(child3.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 220.0f);
  EXPECT_EQ(child3.getSize()[mainAxis], 200.0f);
  EXPECT_EQ(child3.getSize()[crossAxis], 30.0f);
}

TEST_P(
    LayoutTest,
    WrapsItemsAndGrowsSizesIfWrappingIsEnabledAndEachItemCanFitInOneLineAndGrowFactorIsOne) {
  view.setWrap(qui::Wrap::Wrap);

  child1.desiredSize[mainAxis] = 80.0f;
  child1.desiredSize[crossAxis] = 50.0f;
  child2.desiredSize[mainAxis] = 60.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 150.0f;
  child3.desiredSize[crossAxis] = 30.0f;

  qui::FlexView f1, f2, f3;
  f1.setChildren({&child1});
  f2.setChildren({&child2});
  f3.setChildren({&child3});

  view.setChildren({&f1, &f2, &f3});

  f1.setGrow(2);
  f2.setGrow(1);
  f3.setGrow(3);

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[mainAxis] = 200.0f;
  position[crossAxis] = 150.0f;

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 200.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 80.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(f1.getSize()[mainAxis], 120.0f);
  EXPECT_EQ(f1.getSize()[crossAxis], 50.0f);

  EXPECT_EQ(f2.getPosition()[mainAxis], 320.0f);
  EXPECT_EQ(f2.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(f2.getSize()[mainAxis], 80.0f);
  EXPECT_EQ(f2.getSize()[crossAxis], 20.0f);

  EXPECT_EQ(f3.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(f3.getPosition()[crossAxis], 200.0f);
  EXPECT_EQ(f3.getSize()[mainAxis], 200.0f);
  EXPECT_EQ(f3.getSize()[crossAxis], 30.0f);
}

TEST_P(
    LayoutTest,
    WrapsItemsAndAddsSpacingInCrossAxisIfWrappingIsEnabledAndSpacingForCrossAxisIsSpecified) {
  view.setWrap(qui::Wrap::Wrap);

  child1.desiredSize[mainAxis] = 100.0f;
  child1.desiredSize[crossAxis] = 50.0f;
  child2.desiredSize[mainAxis] = 140.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 120.0f;
  child3.desiredSize[crossAxis] = 30.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[crossAxis] = 150.0f;
  position[mainAxis] = 200.0f;

  glm::vec2 spacing{0.0f};
  spacing[crossAxis] = 10.0f;
  view.setSpacing(spacing);

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 140.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 120.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child1.getSize()[mainAxis], 100.0f);
  EXPECT_EQ(child1.getSize()[crossAxis], 50.0f);

  EXPECT_EQ(child2.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 210.0f);
  EXPECT_EQ(child2.getSize()[mainAxis], 140.0f);
  EXPECT_EQ(child2.getSize()[crossAxis], 20.0f);

  EXPECT_EQ(child3.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 240.0f);
  EXPECT_EQ(child3.getSize()[mainAxis], 120.0f);
  EXPECT_EQ(child3.getSize()[crossAxis], 30.0f);
}

TEST_P(
    LayoutTest,
    WrapsItemsAndAddsInBothAxesIfWrappingIsEnabledAndSpacingForBothAxesIsSpecified) {
  view.setWrap(qui::Wrap::Wrap);

  child1.desiredSize[mainAxis] = 80.0f;
  child1.desiredSize[crossAxis] = 50.0f;
  child2.desiredSize[mainAxis] = 60.0f;
  child2.desiredSize[crossAxis] = 20.0f;
  child3.desiredSize[mainAxis] = 120.0f;
  child3.desiredSize[crossAxis] = 30.0f;

  qui::Constraints constraints;
  constraints.max[mainAxis] = 200.0f;
  constraints.max[crossAxis] = 150.0f;

  glm::vec2 position;
  position[crossAxis] = 150.0f;
  position[mainAxis] = 200.0f;

  glm::vec2 spacing;
  spacing[mainAxis] = 5.0f;
  spacing[crossAxis] = 10.0f;
  view.setSpacing(spacing);

  view.layout({constraints, position});

  EXPECT_EQ(view.getSize()[mainAxis], 145.0f);
  EXPECT_EQ(view.getSize()[crossAxis], 90.0f);

  EXPECT_EQ(child1.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child1.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child1.getSize()[mainAxis], 80.0f);
  EXPECT_EQ(child1.getSize()[crossAxis], 50.0f);

  EXPECT_EQ(child2.getPosition()[mainAxis], 285.0f);
  EXPECT_EQ(child2.getPosition()[crossAxis], 150.0f);
  EXPECT_EQ(child2.getSize()[mainAxis], 60.0f);
  EXPECT_EQ(child2.getSize()[crossAxis], 20.0f);

  EXPECT_EQ(child3.getPosition()[mainAxis], 200.0f);
  EXPECT_EQ(child3.getPosition()[crossAxis], 210.0f);
  EXPECT_EQ(child3.getSize()[mainAxis], 120.0f);
  EXPECT_EQ(child3.getSize()[crossAxis], 30.0f);
}

INSTANTIATE_TEST_SUITE_P(
    QuiFlexView, LayoutTest,
    testing::Values(qui::Direction::Row, qui::Direction::Column),
    [](const testing::TestParamInfo<LayoutTest::ParamType> &info) {
      auto direction = std::get<0>(info.param);
      if (direction == qui::Direction::Row) {
        return "Row";
      } else {
        return "Column";
      }
    });
