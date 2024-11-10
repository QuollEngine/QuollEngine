#include "quoll/core/Base.h"
#include "quoll/qui/native/FlexView.h"
#include "quoll-tests/Testing.h"
#include "MockView.h"
#include <imgui.h>

class QuiFlexViewTest : public ::testing::Test {
public:
  QuiFlexViewTest() { view.setChildren({&child1, &child2, &child3}); }

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
  MockView child1, child2, child3;
  qui::FlexView view;
};

TEST_F(QuiFlexViewTest, RendersChildren) {
  view.render();

  EXPECT_EQ(child1.rendered, 1);
  EXPECT_EQ(child2.rendered, 1);
  EXPECT_EQ(child3.rendered, 1);
}

TEST_F(QuiFlexViewTest, HitTestReturnsChildIfPointIsWithinChildBounds) {
  child1.desiredSize.x = 100.0f;
  child1.desiredSize.y = 30.0f;
  child2.desiredSize.x = 50.0f;
  child2.desiredSize.y = 20.0f;
  child3.desiredSize.x = 300.0f;
  child3.desiredSize.y = 40.0f;

  qui::Constraints constraints;
  constraints.min.x = 0.0f;
  constraints.max.x = 1000.0f;
  constraints.min.y = 0.0f;
  constraints.max.y = 1000.0f;

  glm::vec2 position;
  position.x = 200.0f;
  position.y = 150.0f;

  auto output = view.layout({constraints, position});

  EXPECT_EQ(output.size.x, 450.0f);
  EXPECT_EQ(output.size.y, 40.0f);

  // Bounds = [200, 150, 650, 190]
  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({200.0f, 150.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 2);
    EXPECT_EQ(hitResult.path.at(0), &view);
    EXPECT_EQ(hitResult.path.at(1), &child1);
  }

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({300.0f, 180.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 2);
    EXPECT_EQ(hitResult.path.at(0), &view);
    EXPECT_EQ(hitResult.path.at(1), &child1);
  }

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({301.0f, 150.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 2);
    EXPECT_EQ(hitResult.path.at(0), &view);
    EXPECT_EQ(hitResult.path.at(1), &child2);
  }

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({350.0f, 170.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 2);
    EXPECT_EQ(hitResult.path.at(0), &view);
    EXPECT_EQ(hitResult.path.at(1), &child2);
  }

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({351.0f, 150.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 2);
    EXPECT_EQ(hitResult.path.at(0), &view);
    EXPECT_EQ(hitResult.path.at(1), &child3);
  }

  {
    qui::HitTestResult hitResult;
    EXPECT_TRUE(view.hitTest({650.0f, 190.0f}, hitResult));
    EXPECT_EQ(hitResult.path.size(), 2);
    EXPECT_EQ(hitResult.path.at(0), &view);
    EXPECT_EQ(hitResult.path.at(1), &child3);
  }
}

TEST_F(QuiFlexViewTest, HitTestReturnsNullIfPointIsOutsideOfViewBounds) {
  child1.desiredSize.x = 100.0f;
  child1.desiredSize.y = 30.0f;
  child2.desiredSize.x = 50.0f;
  child2.desiredSize.y = 20.0f;
  child3.desiredSize.x = 300.0f;
  child3.desiredSize.y = 40.0f;

  qui::Constraints constraints;
  constraints.min.x = 0.0f;
  constraints.max.x = 1000.0f;
  constraints.min.y = 0.0f;
  constraints.max.y = 1000.0f;

  glm::vec2 position;
  position.x = 200.0f;
  position.y = 150.0f;

  auto output = view.layout({constraints, position});

  qui::HitTestResult hitResult;
  // Bounds = [200, 150, 650, 190]
  EXPECT_FALSE(view.hitTest({200.0f, 149.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({200.0f, 191.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({650.0f, 149.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({650.0f, 191.0f}, hitResult));

  EXPECT_FALSE(view.hitTest({199.0f, 150.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({651.0f, 150.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({199.0f, 190.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({651.0f, 190.0f}, hitResult));

  EXPECT_FALSE(view.hitTest({400.0f, 130.0f}, hitResult));
  EXPECT_FALSE(view.hitTest({700.0f, 240.0f}, hitResult));

  EXPECT_TRUE(hitResult.path.empty());
}
