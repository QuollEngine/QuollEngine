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
