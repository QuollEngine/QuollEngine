#include "quoll/core/Base.h"
#include "quoll/qui/native/ScrollableContentView.h"
#include "quoll-tests/Testing.h"
#include "MockView.h"
#include <imgui.h>

class QuiScrollableContentViewTest : public ::testing::Test {
public:
  QuiScrollableContentViewTest() { view.setChild(&child); }

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
  MockView child;
  qui::ScrollableContentView view;
};

TEST_F(QuiScrollableContentViewTest, RendersChild) {
  view.render();
  EXPECT_EQ(child.rendered, 1);
}

TEST_F(QuiScrollableContentViewTest,
       LayoutSetsMaxConstraintsAsItsSizeAndInputPositionAsItsPosition) {
  qui::Constraints constraints(50.0f, 100.0f, 500.0f, 600.0f);

  child.desiredSize = {600.0f, 900.0f};

  auto output = view.layout({constraints, glm::vec2{200.0f, 300.0f}});

  EXPECT_EQ(output.size.x, 500.0f);
  EXPECT_EQ(output.size.y, 600.0f);
  EXPECT_EQ(view.getSize(), output.size);
  EXPECT_EQ(view.getPosition(), glm::vec2(200.0f, 300.0f));
  EXPECT_EQ(child.position, glm::vec2(200.0f, 300.0f));
}

TEST_F(QuiScrollableContentViewTest,
       LayoutSetsChildSizeAsItsSizeIfConstraintsAreInfinite) {
  qui::Constraints constraints(50.0f, 100.0f, qui::Constraints::Infinity,
                               qui::Constraints::Infinity);

  child.desiredSize = {600.0f, 900.0f};

  auto output = view.layout({constraints, glm::vec2(200.0f, 300.0f)});

  EXPECT_EQ(output.size.x, 600.0f);
  EXPECT_EQ(output.size.y, 900.0f);
  EXPECT_EQ(view.getSize(), output.size);
}

TEST_F(QuiScrollableContentViewTest,
       LayoutConstraintsChildSizeToMinConstraintsIfMaxConstraintsAreInfinite) {
  qui::Constraints constraints(300.0f, 400.0f, qui::Constraints::Infinity,
                               qui::Constraints::Infinity);

  child.desiredSize = {100.0f, 200.0f};

  auto output = view.layout({constraints, glm::vec2(200.0f, 300.0f)});

  EXPECT_EQ(output.size.x, 300.0f);
  EXPECT_EQ(output.size.y, 400.0f);
  EXPECT_EQ(view.getSize(), output.size);
}

TEST_F(QuiScrollableContentViewTest, LayoutSetsChildConstraintsToInfinity) {
  qui::Constraints constraints(50.0f, 100.0f, 500.0f, 600.0f);

  child.desiredSize = {600.0f, 900.0f};

  view.layout({constraints});

  EXPECT_EQ(child.size, child.desiredSize);
  EXPECT_EQ(child.input.constraints.min.x, 0.0f);
  EXPECT_EQ(child.input.constraints.min.y, 0.0f);
  EXPECT_EQ(child.input.constraints.max.x, qui::Constraints::Infinity);
  EXPECT_EQ(child.input.constraints.max.y, qui::Constraints::Infinity);
}

TEST_F(QuiScrollableContentViewTest,
       ScrollIncrementsScrollOffsetByNewDeltaIfOffsetIsWithinBounds) {
  qui::Constraints constraints(50.0f, 100.0f, 500.0f, 600.0f);
  child.desiredSize = {600.0f, 900.0f};
  view.layout({constraints, glm::vec2(200.0f, 300.0f)});

  view.scroll({-50.0f, -100.0f});
  EXPECT_EQ(view.getScrollOffset(), glm::vec2(-50.0f, -100.0f));
}

TEST_F(QuiScrollableContentViewTest,
       ScrollClampsScrollOffsetToZeroIfNewOffsetIsAboveZero) {
  qui::Constraints constraints(50.0f, 100.0f, 500.0f, 600.0f);
  child.desiredSize = {600.0f, 900.0f};
  view.layout({constraints, glm::vec2(200.0f, 300.0f)});

  view.scroll({-50.0f, -100.0f});
  view.scroll({150.0f, 200.0f});
  EXPECT_EQ(view.getScrollOffset(), glm::vec2(0.0f, 0.0f));
}

TEST_F(
    QuiScrollableContentViewTest,
    ScrollClampsScrollOffsetToNegativeContentSizeMinusViewSizeIfNewOffsetIsBelowThatValue) {
  qui::Constraints constraints(50.0f, 100.0f, 500.0f, 600.0f);
  child.desiredSize = {600.0f, 900.0f};
  view.layout({constraints, glm::vec2(200.0f, 300.0f)});

  view.scroll({-105.0f, -310.0f});
  EXPECT_EQ(view.getScrollOffset(), glm::vec2(-100.0f, -300.0f));
}

TEST_F(QuiScrollableContentViewTest, LayoutSetsChildPositionToScrollOffset) {
  qui::Constraints constraints(50.0f, 100.0f, 500.0f, 600.0f);

  child.desiredSize = {600.0f, 900.0f};
  view.layout({constraints, glm::vec2(200.0f, 300.0f)});

  view.scroll({-50.0f, -100.0f});
  view.layout({constraints, glm::vec2(200.0f, 300.0f)});

  EXPECT_EQ(child.position, glm::vec2(150.0, 200.0f));
}

TEST_F(QuiScrollableContentViewTest,
       HitTestReturnsChildIfPointIsWithinViewBoundsAndChildBoundsWithOffset) {
  qui::Constraints constraints(50.0f, 100.0f, 500.0f, 600.0f);
  child.desiredSize = {600.0f, 900.0f};
  view.layout({constraints, glm::vec2{200.0f, 300.0f}});

  EXPECT_EQ(view.hitTest({200.0f, 300.0f}), &child);
  EXPECT_EQ(view.hitTest({200.0f, 900.0f}), &child);
  EXPECT_EQ(view.hitTest({700.0f, 300.0f}), &child);
  EXPECT_EQ(view.hitTest({700.0f, 900.0f}), &child);
  EXPECT_EQ(view.hitTest({500.0f, 400.0f}), &child);
}

TEST_F(QuiScrollableContentViewTest,
       HitTestReturnsViewIfPointIsWithinBoundsButNotWithinChildBounds) {
  qui::Constraints constraints(100.0f, 200.0f, 100.0f, 200.0f);
  child.desiredSize = {50.0f, 500.0f};

  view.layout({constraints, glm::vec2{200.0f, 300.0f}});
  view.scroll({0.0f, -1000.0f});
  view.layout({constraints, glm::vec2{200.0f, 300.0f}});

  EXPECT_EQ(view.hitTest({300.0f, 400.0f}), &view);
}

TEST_F(QuiScrollableContentViewTest,
       HitTestReturnsNullIfPointIsOutsideOfBounds) {
  qui::Constraints constraints(50.0f, 100.0f, 500.0f, 600.0f);
  child.desiredSize = {600.0f, 900.0f};
  view.layout({constraints, glm::vec2{200.0f, 300.0f}});

  EXPECT_EQ(view.hitTest({199.0f, 300.0f}), nullptr);
  EXPECT_EQ(view.hitTest({199.0f, 900.0f}), nullptr);
  EXPECT_EQ(view.hitTest({200.0f, 299.0f}), nullptr);
  EXPECT_EQ(view.hitTest({200.0f, 901.0f}), nullptr);

  EXPECT_EQ(view.hitTest({701.0f, 300.0f}), nullptr);
  EXPECT_EQ(view.hitTest({701.0f, 900.0f}), nullptr);
  EXPECT_EQ(view.hitTest({700.0f, 299.0f}), nullptr);
  EXPECT_EQ(view.hitTest({700.0f, 901.0f}), nullptr);
}
