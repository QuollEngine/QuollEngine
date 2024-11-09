#include "quoll/core/Base.h"
#include "quoll/qui/native/BoxView.h"
#include "quoll-tests/Testing.h"
#include "MockView.h"
#include <imgui.h>

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
  MockView child;
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

  child.desiredSize = {50.0f, 70.0f};

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
  qui::Constraints constraints(50.0f, 65.0f, 100.0f, 100.0f);
  glm::vec2 position{10.0f, 20.0f};

  child.desiredSize = {20.0f, 30.0f};

  // Padding does not affect this
  view.setPadding(qui::EdgeInsets(10.0f, 5.0f, 15.0f, 25.0f));

  view.setChild(&child);
  auto output = view.layout({constraints, position});

  EXPECT_EQ(output.size, glm::vec2(80.0f, 90.0f));
  EXPECT_EQ(view.getSize(), glm::vec2(80.0f, 90.0f));
}

TEST_F(
    QuiBoxViewTest,
    LayoutReturnsMaxConstraintsIfChildSizePlusPaddingIsLargerThanMaxConstraints) {
  qui::Constraints constraints(0.0f, 0.0f, 100.0f, 120.0f);
  glm::vec2 position{10.0f, 20.0f};

  child.desiredSize = {200.0f, 300.0f};

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

TEST_F(QuiBoxViewTest, HitTestReturnsChildIfPointIsWithinChildBounds) {
  view.setWidth(50.0f);
  view.setHeight(100.0f);
  view.setChild(&child);
  view.setPadding(qui::EdgeInsets(5.0f));
  child.desiredSize = {20.0f, 30.0f};
  auto output = view.layout({.position = {40.0f, 50.0f}});

  EXPECT_EQ(view.hitTest({45.0f, 55.0f}), &child);
  EXPECT_EQ(view.hitTest({45.0f, 145.0f}), &child);
  EXPECT_EQ(view.hitTest({85.0f, 55.0f}), &child);
  EXPECT_EQ(view.hitTest({85.0f, 145.0f}), &child);
  EXPECT_EQ(view.hitTest({80.0f, 120.0f}), &child);
}

TEST_F(QuiBoxViewTest,
       HitTestReturnsViewIfPointIsWithinViewBoundsButNotChildBounds) {
  view.setWidth(50.0f);
  view.setHeight(100.0f);
  view.setChild(&child);
  view.setPadding(qui::EdgeInsets(5.0f));
  child.desiredSize = {20.0f, 30.0f};
  auto output = view.layout({.position = {40.0f, 50.0f}});

  EXPECT_EQ(view.hitTest({40.0f, 50.0f}), &view);
  EXPECT_EQ(view.hitTest({40.0f, 150.0f}), &view);
  EXPECT_EQ(view.hitTest({90.0f, 50.0f}), &view);
  EXPECT_EQ(view.hitTest({90.0f, 150.0f}), &view);
}

TEST_F(QuiBoxViewTest, HitTestReturnsNullIfPointIsOutsideOfViewBounds) {
  view.setWidth(50.0f);
  view.setHeight(100.0f);
  view.setChild(&child);
  auto output = view.layout({.position = {40.0f, 50.0f}});

  EXPECT_EQ(view.hitTest({40.0f, 49.0f}), nullptr);
  EXPECT_EQ(view.hitTest({40.0f, 151.0f}), nullptr);
  EXPECT_EQ(view.hitTest({90.0f, 49.0f}), nullptr);
  EXPECT_EQ(view.hitTest({09.0f, 151.0f}), nullptr);

  EXPECT_EQ(view.hitTest({39.0f, 50.0f}), nullptr);
  EXPECT_EQ(view.hitTest({91.0f, 50.0f}), nullptr);
  EXPECT_EQ(view.hitTest({39.0f, 150.0f}), nullptr);
  EXPECT_EQ(view.hitTest({91.0f, 150.0f}), nullptr);

  EXPECT_EQ(view.hitTest({20.0f, 10.0f}), nullptr);
  EXPECT_EQ(view.hitTest({120.0f, 160.0f}), nullptr);
}
