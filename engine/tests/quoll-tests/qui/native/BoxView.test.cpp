#include "quoll/core/Base.h"
#include "quoll/qui/native/BoxView.h"
#include "quoll-tests/Testing.h"

class TestView : public qui::View {
public:
  u32 value = 0;
  qui::LayoutInput input;

  qui::LayoutOutput output;

  qui::LayoutOutput layout(const qui::LayoutInput &input) override {
    this->input = input;
    return output;
  }
};

class QuiBoxViewTest : public ::testing::Test {
public:
  qui::BoxView view;
  TestView child;
};

TEST_F(QuiBoxViewTest, LayoutReturnsMaxConstraintAsSizeIfNoChildIsSet) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};

  auto output = view.layout({constraints});
  EXPECT_EQ(output.size, glm::vec2(100.0f, 100.0f));
}

TEST_F(QuiBoxViewTest, LayoutReturnsSizeIfSpecifiedSizeIsWithinConstraints) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};

  view.setWidth(50.0f);
  view.setHeight(70.0f);
  auto output = view.layout({constraints});
  EXPECT_EQ(output.size, glm::vec2(50.0f, 70.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsMinConstraintsAsSizeIfSpecifiedSizeIsSmallerThanMinConstraints) {
  qui::Constraints constraints{40.0f, 50.0f, 100.0f, 100.0f};

  view.setWidth(20.0f);
  view.setHeight(30.0f);
  auto output = view.layout({constraints});
  EXPECT_EQ(output.size, glm::vec2(50.0f, 40.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsMaxConstraintsAsSizeIfSpecifiedSizeIsLargerThanMaxConstraints) {
  qui::Constraints constraints{0.0f, 0.0f, 120.0f, 100.0f};

  view.setWidth(200.0f);
  view.setHeight(300.0f);
  auto output = view.layout({constraints});
  EXPECT_EQ(output.size, glm::vec2(100.0f, 120.0f));
}

TEST_F(QuiBoxViewTest,
       LayoutPassesInputConstraintsToChildIfNoPaddingOrCustomDimensions) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};

  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.minHeight, 0.0f);
  EXPECT_EQ(child.input.constraints.minWidth, 0.0f);
  EXPECT_EQ(child.input.constraints.maxHeight, 100.0f);
  EXPECT_EQ(child.input.constraints.maxWidth, 100.0f);
}

TEST_F(QuiBoxViewTest,
       LayoutPassesShrinkedConstraintsToChildIfPaddingIsNotZero) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};

  view.setChild(&child);
  view.setPadding({10.0f, 5.0f, 20.0f, 40.0f});
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.minHeight, 0.0f);
  EXPECT_EQ(child.input.constraints.minWidth, 0.0f);
  EXPECT_EQ(child.input.constraints.maxHeight, 70.0f);
  EXPECT_EQ(child.input.constraints.maxWidth, 55.0f);
}

TEST_F(
    QuiBoxViewTest,
    LayoutPassesDimensionsIfDimensionsAreProvidedAndAreWithinTheConstraints) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};

  view.setWidth(50.0f);
  view.setHeight(40.0f);
  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.minHeight, 40.0f);
  EXPECT_EQ(child.input.constraints.minWidth, 50.0f);
  EXPECT_EQ(child.input.constraints.maxHeight, 40.0f);
  EXPECT_EQ(child.input.constraints.maxWidth, 50.0f);
}

TEST_F(
    QuiBoxViewTest,
    LayoutPassesDimensionsClampedToMinConstraintsIfDimensionsAreSmallerThanMinConstraints) {
  qui::Constraints constraints{40.0f, 50.0f, 100.0f, 100.0f};

  view.setWidth(30.0f);
  view.setHeight(20.0f);
  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.minHeight, 40.0f);
  EXPECT_EQ(child.input.constraints.minWidth, 50.0f);
  EXPECT_EQ(child.input.constraints.maxHeight, 40.0f);
  EXPECT_EQ(child.input.constraints.maxWidth, 50.0f);
}

TEST_F(
    QuiBoxViewTest,
    LayoutPassesDimensionsClampedToMaxConstraintsIfDimensionsAreBiggerThanMaxConstraints) {
  qui::Constraints constraints{40.0f, 50.0f, 120.0f, 160.0f};

  view.setWidth(200.0f);
  view.setHeight(300.0f);
  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.minHeight, 120.0f);
  EXPECT_EQ(child.input.constraints.minWidth, 160.0f);
  EXPECT_EQ(child.input.constraints.maxHeight, 120.0f);
  EXPECT_EQ(child.input.constraints.maxWidth, 160.0f);
}

TEST_F(QuiBoxViewTest, LayoutPassesShrinkedDimensionsIfPaddingIsProvided) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};

  view.setWidth(70.0f);
  view.setHeight(60.0f);
  view.setPadding({10.0f, 5.0f, 15.0f, 20.0f});
  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.minHeight, 35.0f);
  EXPECT_EQ(child.input.constraints.minWidth, 45.0f);
  EXPECT_EQ(child.input.constraints.maxHeight, 35.0f);
  EXPECT_EQ(child.input.constraints.maxWidth, 45.0f);
}

TEST_F(QuiBoxViewTest, LayoutPassesInputConstraintsToChildIfNoPadding) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};
  glm::vec2 position{10.0f, 20.0f};

  view.setChild(&child);
  view.layout({constraints, position});

  EXPECT_EQ(child.input.position, position);
}

TEST_F(QuiBoxViewTest, LayoutPassesModifiedPositionIfStartPaddingIsSpecified) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};
  glm::vec2 position{10.0f, 20.0f};

  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 20.0f, 40.0f));

  view.setChild(&child);
  view.layout({constraints, position});

  EXPECT_EQ(child.input.position.x, 15.0f);
  EXPECT_EQ(child.input.position.y, 30.0f);
}

TEST_F(QuiBoxViewTest, LayoutPassesInputPositionIfDimensionsAreSpecified) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};
  glm::vec2 position{10.0f, 20.0f};

  view.setWidth(20.0f);
  view.setHeight(30.0f);
  view.setChild(&child);
  view.layout({constraints, position});

  EXPECT_EQ(child.input.position, position);
}

TEST_F(QuiBoxViewTest, LayoutReturnsChildSizeAsItsOwnSizeIfNoSizeIsProvided) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};
  glm::vec2 position{10.0f, 20.0f};

  child.output.size = {50.0f, 70.0f};

  // Padding does not affect this
  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 20.0f, 40.0f));

  view.setChild(&child);
  auto output = view.layout({constraints, position});

  EXPECT_EQ(output.size, glm::vec2(50.0f, 70.0f));
}

TEST_F(QuiBoxViewTest,
       LayoutReturnsMinConstraintsIfChildSizeIsSmallerThanMinConstraints) {
  qui::Constraints constraints{40.0f, 50.0f, 100.0f, 100.0f};
  glm::vec2 position{10.0f, 20.0f};

  child.output.size = {20.0f, 30.0f};

  // Padding does not affect this
  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 20.0f, 40.0f));

  view.setChild(&child);
  auto output = view.layout({constraints, position});

  EXPECT_EQ(output.size, glm::vec2(50.0f, 40.0f));
}

TEST_F(QuiBoxViewTest,
       LayoutReturnsMaxConstraintsIfChildSizeIsLargerThanMaxConstraints) {
  qui::Constraints constraints{0.0f, 0.0f, 120.0f, 100.0f};
  glm::vec2 position{10.0f, 20.0f};

  child.output.size = {200.0f, 300.0f};

  // Padding does not affect this
  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 20.0f, 40.0f));

  view.setChild(&child);
  auto output = view.layout({constraints, position});

  EXPECT_EQ(output.size, glm::vec2(100.0f, 120.0f));
}

TEST_F(QuiBoxViewTest,
       LayoutReturnsSizeWithinConstraintsIfSpecifiedSizeIsWithinConstraints) {
  qui::Constraints constraints{0.0f, 0.0f, 100.0f, 100.0f};

  view.setWidth(70.0f);
  view.setHeight(50.0f);

  view.setChild(&child);
  auto output = view.layout({constraints});

  EXPECT_EQ(output.size, glm::vec2(70.0f, 50.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsSizeClampedToMinConstraintsIfSpecifiedSizeIsSmallerThanMinConstraints) {
  qui::Constraints constraints{50.0f, 40.0f, 100.0f, 100.0f};

  view.setWidth(20.0f);
  view.setHeight(30.0f);

  view.setChild(&child);
  auto output = view.layout({constraints});

  EXPECT_EQ(output.size, glm::vec2(40.0f, 50.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsSizeClampedToMaxConstraintsIfSpecifiedSizeIsLargerThanMaxConstraints) {
  qui::Constraints constraints{0.0f, 0.0f, 120.0f, 100.0f};

  view.setWidth(200.0f);
  view.setHeight(300.0f);

  view.setChild(&child);
  auto output = view.layout({constraints});

  EXPECT_EQ(output.size, glm::vec2(100.0f, 120.0f));
}
