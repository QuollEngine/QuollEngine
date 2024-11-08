#include "quoll/core/Base.h"
#include "quoll/qui/native/BoxView.h"
#include "quoll-tests/Testing.h"
#include <imgui.h>

class TestView : public qui::View {
public:
  u32 value = 0;
  qui::LayoutInput input;

  qui::LayoutOutput output;

  u32 rendered = 0;

  void render() override { rendered++; }

  qui::LayoutOutput layout(const qui::LayoutInput &input) override {
    this->input = input;
    return output;
  }
};

class QuiBoxViewTest : public ::testing::Test {
public:
  void SetUp() override {
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(800, 600);
    io.Fonts->Build();
    ImGui::NewFrame();
  }

  void TearDown() override {
    ImGui::Render();
    ImGui::DestroyContext();
  }

public:
  qui::BoxView view;
  TestView child;
};

TEST_F(QuiBoxViewTest, LayoutReturnsMaxConstraintAsSizeIfNoChildIsSet) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);

  auto output = view.layout({constraints});
  EXPECT_EQ(output.size, glm::vec2(100.0f, 100.0f));
}

TEST_F(QuiBoxViewTest, LayoutReturnsSizeIfSpecifiedSizeIsWithinConstraints) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);

  view.setWidth(50.0f);
  view.setHeight(70.0f);
  auto output = view.layout({constraints});
  EXPECT_EQ(output.size, glm::vec2(50.0f, 70.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsMinConstraintsAsSizeIfSpecifiedSizeIsSmallerThanMinConstraints) {
  qui::Constraints constraints(50.0f, 40.0f, 100.0f, 100.0f);

  view.setWidth(20.0f);
  view.setHeight(30.0f);
  auto output = view.layout({constraints});
  EXPECT_EQ(output.size, glm::vec2(50.0f, 40.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsMaxConstraintsAsSizeIfSpecifiedSizeIsLargerThanMaxConstraints) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 120.0f);

  view.setWidth(200.0f);
  view.setHeight(300.0f);
  auto output = view.layout({constraints});
  EXPECT_EQ(output.size, glm::vec2(100.0f, 120.0f));
}

TEST_F(QuiBoxViewTest,
       LayoutPassesInputConstraintsToChildIfNoPaddingOrCustomDimensions) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);

  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.min.y, 0.0f);
  EXPECT_EQ(child.input.constraints.min.x, 0.0f);
  EXPECT_EQ(child.input.constraints.max.y, 100.0f);
  EXPECT_EQ(child.input.constraints.max.x, 100.0f);
}

TEST_F(QuiBoxViewTest,
       LayoutPassesShrinkedConstraintsToChildIfPaddingIsNotZero) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);

  view.setChild(&child);
  view.setPadding({10.0f, 5.0f, 20.0f, 40.0f});
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.min.y, 0.0f);
  EXPECT_EQ(child.input.constraints.min.x, 0.0f);
  EXPECT_EQ(child.input.constraints.max.y, 70.0f);
  EXPECT_EQ(child.input.constraints.max.x, 55.0f);
}

TEST_F(
    QuiBoxViewTest,
    LayoutPassesDimensionsIfDimensionsAreProvidedAndAreWithinTheConstraints) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);

  view.setWidth(50.0f);
  view.setHeight(40.0f);
  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.min.y, 40.0f);
  EXPECT_EQ(child.input.constraints.min.x, 50.0f);
  EXPECT_EQ(child.input.constraints.max.y, 40.0f);
  EXPECT_EQ(child.input.constraints.max.x, 50.0f);
}

TEST_F(
    QuiBoxViewTest,
    LayoutPassesDimensionsClampedToMinConstraintsIfDimensionsAreSmallerThanMinConstraints) {
  qui::Constraints constraints(50.0f, 40.0f, 100.0f, 100.0f);

  view.setWidth(30.0f);
  view.setHeight(20.0f);
  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.min.y, 40.0f);
  EXPECT_EQ(child.input.constraints.min.x, 50.0f);
  EXPECT_EQ(child.input.constraints.max.y, 40.0f);
  EXPECT_EQ(child.input.constraints.max.x, 50.0f);
}

TEST_F(
    QuiBoxViewTest,
    LayoutPassesDimensionsClampedToMaxConstraintsIfDimensionsAreBiggerThanMaxConstraints) {
  qui::Constraints constraints(50.0f, 40.0f, 160.0f, 120.0f);

  view.setWidth(200.0f);
  view.setHeight(300.0f);
  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.min.y, 120.0f);
  EXPECT_EQ(child.input.constraints.min.x, 160.0f);
  EXPECT_EQ(child.input.constraints.max.y, 120.0f);
  EXPECT_EQ(child.input.constraints.max.x, 160.0f);
}

TEST_F(QuiBoxViewTest, LayoutPassesShrinkedDimensionsIfPaddingIsProvided) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);

  view.setWidth(70.0f);
  view.setHeight(60.0f);
  view.setPadding({10.0f, 5.0f, 15.0f, 20.0f});
  view.setChild(&child);
  view.layout({constraints});

  EXPECT_EQ(child.input.constraints.min.y, 35.0f);
  EXPECT_EQ(child.input.constraints.min.x, 45.0f);
  EXPECT_EQ(child.input.constraints.max.y, 35.0f);
  EXPECT_EQ(child.input.constraints.max.x, 45.0f);
}

TEST_F(QuiBoxViewTest, LayoutPassesInputConstraintsToChildIfNoPadding) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);
  glm::vec2 position{10.0f, 20.0f};

  view.setChild(&child);
  view.layout({constraints, position});

  EXPECT_EQ(child.input.position, position);
}

TEST_F(QuiBoxViewTest, LayoutPassesModifiedPositionIfStartPaddingIsSpecified) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);
  glm::vec2 position{10.0f, 20.0f};

  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 20.0f, 40.0f));

  view.setChild(&child);
  view.layout({constraints, position});

  EXPECT_EQ(child.input.position.x, 15.0f);
  EXPECT_EQ(child.input.position.y, 30.0f);
}

TEST_F(QuiBoxViewTest, LayoutPassesInputPositionIfDimensionsAreSpecified) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);
  glm::vec2 position{10.0f, 20.0f};

  view.setWidth(20.0f);
  view.setHeight(30.0f);
  view.setChild(&child);
  view.layout({constraints, position});

  EXPECT_EQ(child.input.position, position);
}

TEST_F(QuiBoxViewTest, LayoutReturnsChildSizePlusPaddingIfNoSizeIsProvided) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);
  glm::vec2 position{10.0f, 20.0f};

  child.output.size = {50.0f, 70.0f};

  // Padding does not affect this
  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 20.0f, 40.0f));

  view.setChild(&child);
  auto output = view.layout({constraints, position});

  EXPECT_EQ(output.size, glm::vec2(95.0f, 100.0f));
  EXPECT_EQ(view.getSize(), glm::vec2(95.0f, 100.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsMinConstraintsIfChildSizePlusPaddingIsSmallerThanMinConstraints) {
  qui::Constraints constraints(70.0f, 65.0f, 100.0f, 100.0f);
  glm::vec2 position{10.0f, 20.0f};

  child.output.size = {20.0f, 30.0f};

  // Padding does not affect this
  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 20.0f, 40.0f));

  view.setChild(&child);
  auto output = view.layout({constraints, position});

  EXPECT_EQ(output.size, glm::vec2(70.0f, 65.0f));
  EXPECT_EQ(view.getSize(), glm::vec2(70.0f, 65.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsMaxConstraintsIfChildSizePlusPaddingIsLargerThanMaxConstraints) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 120.0f);
  glm::vec2 position{10.0f, 20.0f};

  child.output.size = {200.0f, 300.0f};

  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 20.0f, 40.0f));

  view.setChild(&child);
  auto output = view.layout({constraints, position});

  EXPECT_EQ(output.size, glm::vec2(100.0f, 120.0f));
  EXPECT_EQ(view.getSize(), glm::vec2(100.0f, 120.0f));
}

TEST_F(QuiBoxViewTest,
       LayoutReturnsSizeWithinConstraintsIfSpecifiedSizeIsWithinConstraints) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 100.0f);

  view.setWidth(70.0f);
  view.setHeight(50.0f);

  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 20.0f, 40.0f));

  view.setChild(&child);
  auto output = view.layout({constraints});

  EXPECT_EQ(output.size, glm::vec2(70.0f, 50.0f));
  EXPECT_EQ(view.getSize(), glm::vec2(70.0f, 50.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsSizeClampedToMinConstraintsIfSpecifiedSizeIsSmallerThanMinConstraints) {
  qui::Constraints constraints(40.0f, 50.0f, 100.0f, 100.0f);

  view.setWidth(20.0f);
  view.setHeight(30.0f);

  view.setChild(&child);
  auto output = view.layout({constraints});

  EXPECT_EQ(output.size, glm::vec2(40.0f, 50.0f));
  EXPECT_EQ(view.getSize(), glm::vec2(40.0f, 50.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsSizeClampedToMaxConstraintsIfSpecifiedSizeIsLargerThanMaxConstraints) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 120.0f);

  view.setWidth(200.0f);
  view.setHeight(300.0f);

  view.setChild(&child);
  auto output = view.layout({constraints});

  EXPECT_EQ(output.size, glm::vec2(100.0f, 120.0f));
  EXPECT_EQ(view.getSize(), glm::vec2(100.0f, 120.0f));
}

TEST_F(QuiBoxViewTest, LayoutSetsInputPositionAsViewPositionIfChildIsNotSet) {
  glm::vec2 position{10.0f, 20.0f};
  view.layout({{}, position});

  EXPECT_EQ(view.getPosition(), position);
}

TEST_F(QuiBoxViewTest, LayoutSetsInputPositionAsViewPositionIfChildIsSet) {
  glm::vec2 position{10.0f, 20.0f};
  view.setChild(&child);
  view.layout({{}, position});

  EXPECT_EQ(view.getPosition(), position);
}

TEST_F(QuiBoxViewTest, RenderWorksIfNoChild) { view.render(); }

TEST_F(QuiBoxViewTest, RenderCallsChildRender) {
  view.setChild(&child);
  view.render();

  EXPECT_EQ(child.rendered, 1);
}

TEST_F(QuiBoxViewTest, HitTestReturnsTrueIfPointIsWithinViewBounds) {
  view.setWidth(50.0f);
  view.setHeight(100.0f);
  view.setChild(&child);
  auto output = view.layout({.position = {40.0f, 50.0f}});

  EXPECT_TRUE(view.hitTest({40.0f, 50.0f}));
  EXPECT_TRUE(view.hitTest({40.0f, 150.0f}));
  EXPECT_TRUE(view.hitTest({90.0f, 50.0f}));
  EXPECT_TRUE(view.hitTest({90.0f, 150.0f}));
  EXPECT_TRUE(view.hitTest({80.0f, 120.0f}));
}

TEST_F(QuiBoxViewTest, HitTestReturnsFalseIfPointIsOutsideOfViewBounds) {
  view.setWidth(50.0f);
  view.setHeight(100.0f);
  view.setChild(&child);
  auto output = view.layout({.position = {40.0f, 50.0f}});

  EXPECT_FALSE(view.hitTest({40.0f, 49.0f}));
  EXPECT_FALSE(view.hitTest({40.0f, 151.0f}));
  EXPECT_FALSE(view.hitTest({90.0f, 49.0f}));
  EXPECT_FALSE(view.hitTest({09.0f, 151.0f}));

  EXPECT_FALSE(view.hitTest({39.0f, 50.0f}));
  EXPECT_FALSE(view.hitTest({91.0f, 50.0f}));
  EXPECT_FALSE(view.hitTest({39.0f, 150.0f}));
  EXPECT_FALSE(view.hitTest({91.0f, 150.0f}));

  EXPECT_FALSE(view.hitTest({20.0f, 10.0f}));
  EXPECT_FALSE(view.hitTest({120.0f, 160.0f}));
}
