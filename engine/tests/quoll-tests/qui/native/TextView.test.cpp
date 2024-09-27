#include "quoll/core/Base.h"
#include "quoll/qui/native/TextView.h"
#include "quoll-tests/Testing.h"
#include <imgui.h>

class QuiTextViewTest : public testing::Test {
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
};

TEST_F(QuiTextViewTest, LayoutCalculatesTextSizeBasedOnConstraintMaxWidth) {
  qui::TextView view;
  view.setText("Hello world");

  qui::Constraints constraints(0.0f, 0.0f, 50.0f, 100.0f);

  auto output = view.layout({.constraints = constraints, .position = {0, 0}});
  EXPECT_EQ(output.size, glm::vec2(35.0f, 26.0f));
}

TEST_F(QuiTextViewTest, LayoutConstraintTextSizeBasedOnInputConstraints) {
  qui::TextView view;
  view.setText("Hello world");

  // Imgui calculated text size is (35.0f, 26.0f)
  qui::Constraints constraints(40.0f, 30.0f, 50.0f, 60.0f);
  auto output = view.layout({.constraints = constraints, .position = {0, 0}});
  EXPECT_EQ(output.size, glm::vec2(40.0f, 30.0f));
}

TEST_F(QuiTextViewTest, LayoutSetsInputPositionAsTextViewPosition) {
  qui::TextView view;
  view.setText("Hello world");

  view.layout({.constraints = {}, .position = {40.0f, 50.0f}});
  EXPECT_EQ(view.getPosition(), glm::vec2(40.0f, 50.0f));
}
