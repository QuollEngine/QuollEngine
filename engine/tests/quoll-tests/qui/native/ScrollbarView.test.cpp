#include "quoll/core/Base.h"
#include "quoll/qui/native/ScrollbarView.h"
#include "quoll-tests/Testing.h"
#include "MockView.h"
#include <imgui.h>

class QuiScrollbarViewTest : public ::testing::Test {
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
  qui::ScrollbarView view;
};

TEST_F(QuiScrollbarViewTest,
       LayoutSetsCalculatesPositionBasedOnInputPositionAndThickness) {
  view.setThickness(5.0f);
  view.layout({qui::Constraints(), {50.0f, 60.0f}});

  EXPECT_EQ(view.getPosition(), glm::vec2(45.0f, 60.0f));
}

TEST_F(QuiScrollbarViewTest,
       LayoutSetsSizeBasedOnInputConstraintsAndThickness) {
  view.setThickness(5.0f);
  view.layout({qui::Constraints(0.0f, 100.0f, 0.0f, 100.0f), {50.0f, 60.0f}});

  EXPECT_EQ(view.getSize(), glm::vec2(5.0f, 100.0f));
}

TEST_F(QuiScrollbarViewTest, HitTestReturnsViewIfPointIsInside) {
  view.setThickness(5.0f);
  view.layout({qui::Constraints(0.0f, 100.0f, 0.0f, 100.0f), {50.0f, 60.0f}});

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({45.0f, 160.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 1);
    EXPECT_EQ(hitResult.path.at(0), &view);
  }

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({45.0f, 60.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 1);
    EXPECT_EQ(hitResult.path.at(0), &view);
  }

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({50.0f, 60.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 1);
    EXPECT_EQ(hitResult.path.at(0), &view);
  }

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({50.0f, 160.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 1);
    EXPECT_EQ(hitResult.path.at(0), &view);
  }

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({47.0f, 120.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 1);
    EXPECT_EQ(hitResult.path.at(0), &view);
  }
}

TEST_F(QuiScrollbarViewTest, HitTestReturnsNullIfPointIsOutside) {
  view.setThickness(5.0f);
  view.layout({qui::Constraints(0.0f, 100.0f, 0.0f, 100.0f), {50.0f, 60.0f}});

  qui::HitTestResult hitResult;

  EXPECT_FALSE(view.hitTest({44.0f, 160.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({44.0f, 60.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({45.0f, 161.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({45.0f, 59.0f}, hitResult));

  EXPECT_FALSE(view.hitTest({51.0f, 160.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({51.0f, 60.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({50.0f, 161.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({50.0f, 59.0f}, hitResult));

  EXPECT_TRUE(hitResult.path.empty());
}
