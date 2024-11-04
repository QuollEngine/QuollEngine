#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/qui/Components.h"
#include "DebugPanel.h"
#include "ImguiDebugLayer.h"

namespace quoll {

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
static constexpr auto ColoredText =
    qui::component([](qui::Value<quoll::String> text) {
      return qui::Box(qui::Text(text).color(qui::Color::Yellow))
          .background(qui::Color::Red)
          .width(40.0f)
          .height(40.0f);
    });

static constexpr auto DemoQuiFlexbox = qui::component([]() {
  return qui::Box(qui::Flex({
                                ColoredText("1"),
                                ColoredText("2"),
                                ColoredText("3"),
                                ColoredText("4"),
                                ColoredText("5"),
                                ColoredText("6"),
                                ColoredText("7"),
                                ColoredText("8"),
                            })
                      .wrap(qui::Wrap::Wrap)
                      .spacing(glm::vec2(5.0f)))
      .width(200.0f)
      .height(300.0f);
});

struct ButtonStyle {
  qui::Color color;
  qui::Color backgroundColor;
};

static constexpr auto ColoredButton = qui::component(
    [](qui::Scope &scope, qui::Value<quoll::String> text,
       qui::Value<ButtonStyle> style, qui::Value<ButtonStyle> hoverStyle,
       qui::Value<ButtonStyle> activeStyle) {
      auto hovered = scope.signal(false);
      auto clicked = scope.signal(false);

      auto color =
          scope.computation([hovered, clicked, hoverStyle, activeStyle, style] {
            if (hovered()) {
              return hoverStyle().color;
            }

            return clicked() ? activeStyle().color : style().color;
          });

      auto backgroundColor =
          scope.computation([hovered, clicked, hoverStyle, activeStyle, style] {
            if (hovered()) {
              return hoverStyle().backgroundColor;
            }

            return clicked() ? activeStyle().backgroundColor
                             : style().backgroundColor;
          });

      return qui::Pressable(qui::Box(qui::Text(text).color(color))
                                .padding(qui::EdgeInsets(5.0f))
                                .background(backgroundColor))
          .onHoverIn([hovered](const auto &) mutable { hovered.set(true); })
          .onHoverOut([hovered](const auto &) mutable { hovered.set(false); })
          .onPress(
              [clicked](const auto &) mutable { clicked.set(!clicked()); });
    });

static constexpr auto DemoPressable = qui::component([]() {
  return ColoredButton(
      "Hello world",
      ButtonStyle{.color = qui::Color::White, .backgroundColor = 0x2e86deff},
      ButtonStyle{.color = qui::Color::White, .backgroundColor = 0x54a0ffff},
      ButtonStyle{.color = qui::Color::White, .backgroundColor = 0xee5253ff});
});

static constexpr auto DemoQui = qui::component([]() {
  return qui::Flex({qui::Box(qui::Text("Red text on a yellow background")
                                 .color(qui::Color::Red))
                        .padding(qui::EdgeInsets(5.0f))
                        .background(qui::Color::Yellow)
                        .width(100.0f)
                        .borderRadius(5.0f),

                    qui::Box(qui::Text("Text with resizable sizing"))
                        .background(qui::Color::White)
                        .padding(qui::EdgeInsets(5.0f)),

                    DemoQuiFlexbox(), DemoPressable()})
      .spacing(glm::vec2{10.0f, 0.0f});
});
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

ImguiDebugLayer::ImguiDebugLayer(std::vector<debug::DebugPanel *> panels)
    : mPanels(panels) {
  mDemoTree = qui::Qui::createTree(DemoQui());
}

void ImguiDebugLayer::renderMenu() {
  if (ImGui::BeginMenu("Debug")) {
    for (auto *panel : mPanels) {
      panel->onRenderMenu();
    }

    ImGui::MenuItem("Imgui demo", nullptr, &mDemoWindowOpen);
    ImGui::MenuItem("Qui demo", nullptr, &mQuiOpen);

    ImGui::EndMenu();
  }
}

void ImguiDebugLayer::render() {
  for (auto *panel : mPanels) {
    panel->onRender();
  }

  renderDemoWindow();
  renderQui();
}

void ImguiDebugLayer::renderDemoWindow() {
  if (!mDemoWindowOpen)
    return;

  ImGui::ShowDemoWindow(&mDemoWindowOpen);
}

void ImguiDebugLayer::renderQui() {
  if (!mQuiOpen)
    return;

  if (ImGui::Begin("Qui", &mQuiOpen, ImGuiWindowFlags_NoDocking)) {
    auto size = ImGui::GetWindowSize();
    auto pos = ImGui::GetCursorScreenPos();
    qui::Qui::render(mDemoTree, {pos.x, pos.y}, {size.x, size.y});
  }
  ImGui::End();
}

} // namespace quoll
