#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/imgui/Imgui.h"

#include "Theme.h"

namespace quoll::editor {
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

/**
 * @brief Convert Srgb color to linear
 *
 * This function automatically converts
 * gamma corrected nonlinear values to linear
 * ones; so, it is easier to apply colors
 * from graphics design applications to here
 * without manually performing calculations.
 *
 * @param value Srgb value
 * @return Linear value
 */
static constexpr f32 SrgbToLinear(f32 value) {
  if (value <= 0.0031308f) {
    return value / 12.92f;
  }

  return pow((value + 0.055f) / 1.055f, 2.4f);
}

static constexpr ImColor rgb(i32 r, i32 g, i32 b) { return ImColor(r, g, b); }

static constexpr ImColor rgba(i32 r, i32 g, i32 b, i32 a) {
  return ImColor(r, g, b, a);
}

static ImVec4 SrgbToLinear(ImColor color) {
  return {SrgbToLinear(color.Value.x), SrgbToLinear(color.Value.y),
          SrgbToLinear(color.Value.z), color.Value.w};
}

static constexpr f32 FontSize = 18.0f;

static constexpr usize NumFonts = 2;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static std::array<ImFont *, NumFonts> Fonts{};

static const std::unordered_map<ThemeColor, ImColor> Colors{
    // Base
    {ThemeColor::White, rgb(255, 255, 255)},
    {ThemeColor::Black, rgb(0, 0, 0)},
    {ThemeColor::Transparent, rgba(0, 0, 0, 0)},

    // Charcoal
    {ThemeColor::Charcoal300, rgb(49, 49, 53)},
    {ThemeColor::Charcoal400, rgb(44, 44, 48)},
    {ThemeColor::Charcoal500, rgb(39, 39, 43)},
    {ThemeColor::Charcoal600, rgb(34, 34, 37)},
    {ThemeColor::Charcoal700, rgb(32, 32, 35)},
    {ThemeColor::Charcoal800, rgb(27, 27, 29)},

    // Sienna
    {ThemeColor::Sienna500, rgb(210, 103, 47)},
    {ThemeColor::Sienna600, rgb(168, 82, 36)},

    // Misc
    {ThemeColor::ModalBackdrop, rgba(0, 0, 0, 220)},

    // End
};

static constexpr ThemeStyles Styles{.itemSpacing = ImVec2(8.0f, 8.0f),
                                    .sectionRounding = ImVec2(8.0f, 8.0f),
                                    .windowPadding = ImVec2(10.0f, 10.0f),
                                    .framePadding = ImVec2(10.0f, 10.0f),
                                    .childRounding = 4.0f};

static void setImguiStyles() {
  auto &style = ImGui::GetStyle();

  // Separator
  style.Colors[ImGuiCol_Separator] = Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_SeparatorHovered] =
      Theme::getColor(ThemeColor::Sienna600);
  style.Colors[ImGuiCol_SeparatorActive] =
      Theme::getColor(ThemeColor::Sienna600);
  style.Colors[ImGuiCol_Border] = Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_DockingPreview] =
      Theme::getColor(ThemeColor::Sienna600);

  // All items
  style.Colors[ImGuiCol_FrameBg] = Theme::getColor(ThemeColor::Charcoal600);
  style.Colors[ImGuiCol_FrameBgHovered] =
      Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_FrameBgActive] =
      Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_PopupBg] = Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_ChildBg] = Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_ModalWindowDimBg] =
      Theme::getColor(ThemeColor::ModalBackdrop);
  style.ItemSpacing = Styles.itemSpacing;
  style.ItemInnerSpacing = ImVec2(8.0f, 8.0f);
  style.FrameRounding = 4.0f;
  style.FramePadding = Styles.framePadding;
  style.FrameBorderSize = 0.0f;
  style.PopupBorderSize = 0.0f;
  style.PopupRounding = 4.0f;
  style.ChildRounding = Styles.childRounding;
  style.WindowMenuButtonPosition = ImGuiDir_None;

  // Tables
  style.Colors[ImGuiCol_TableBorderLight] =
      Theme::getColor(ThemeColor::Transparent);
  style.Colors[ImGuiCol_TableBorderStrong] =
      Theme::getColor(ThemeColor::Transparent);
  style.Colors[ImGuiCol_TableHeaderBg] =
      Theme::getColor(ThemeColor::Charcoal500);
  style.Colors[ImGuiCol_TableRowBg] = Theme::getColor(ThemeColor::Charcoal600);
  style.Colors[ImGuiCol_TableRowBgAlt] =
      Theme::getColor(ThemeColor::Charcoal500);
  style.CellPadding = ImVec2(8.0f, 8.0f);

  // Window
  style.Colors[ImGuiCol_WindowBg] = Theme::getColor(ThemeColor::Charcoal700);
  style.Colors[ImGuiCol_MenuBarBg] = Theme::getColor(ThemeColor::Charcoal700);
  style.Colors[ImGuiCol_TitleBg] = Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_TitleBgCollapsed] =
      Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_TitleBgActive] =
      Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_ResizeGrip] = ImColor(255, 255, 255, 125);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(255, 255, 255, 255);
  style.Colors[ImGuiCol_ResizeGripActive] = ImColor(255, 255, 255, 255);
  style.WindowRounding = 8.0f;
  style.WindowPadding = Styles.windowPadding;
  style.WindowBorderSize = 0.0f;

  // Tabs
  style.Colors[ImGuiCol_Tab] = Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_TabUnfocused] =
      Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_TabHovered] = Theme::getColor(ThemeColor::Charcoal700);
  style.Colors[ImGuiCol_TabActive] = Theme::getColor(ThemeColor::Charcoal700);
  style.Colors[ImGuiCol_TabUnfocusedActive] =
      Theme::getColor(ThemeColor::Charcoal700);

  // Headers
  // Used by collapsing header
  style.Colors[ImGuiCol_Header] = Theme::getColor(ThemeColor::Transparent);
  style.Colors[ImGuiCol_HeaderActive] = Theme::getColor(ThemeColor::Sienna600);
  style.Colors[ImGuiCol_HeaderHovered] = Theme::getColor(ThemeColor::Sienna600);

  // Buttons
  style.Colors[ImGuiCol_Button] = Theme::getColor(ThemeColor::Charcoal800);
  style.Colors[ImGuiCol_ButtonHovered] = Theme::getColor(ThemeColor::Sienna600);
  style.Colors[ImGuiCol_ButtonActive] = Theme::getColor(ThemeColor::Sienna600);

  style.Colors[ImGuiCol_CheckMark] = Theme::getColor(ThemeColor::White);
  style.Colors[ImGuiCol_SliderGrab] = Theme::getColor(ThemeColor::Charcoal400);
  style.Colors[ImGuiCol_SliderGrabActive] =
      Theme::getColor(ThemeColor::Charcoal300);

  style.Colors[ImGuiCol_TextSelectedBg] =
      Theme::getColor(ThemeColor::Sienna600);
  style.Colors[ImGuiCol_DragDropTarget] =
      Theme::getColor(ThemeColor::Sienna600);
}

static void addFonts() {
  auto &io = ImGui::GetIO();

  Path defaultFontPath = Engine::getFontsPath() / "Roboto-Regular.ttf";
  Fonts.at(0) =
      io.Fonts->AddFontFromFileTTF(defaultFontPath.string().c_str(), FontSize);

  Path boldFontPath =
      std::filesystem::current_path() / "assets" / "fonts" / "Roboto-Bold.ttf";
  Fonts.at(1) =
      io.Fonts->AddFontFromFileTTF(boldFontPath.string().c_str(), FontSize);

  for (usize i = 0; i < Fonts.size(); ++i) {
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = FontSize;
    config.DstFont = Fonts.at(i);

    static constexpr std::array<ImWchar, 3> IconRanges{0xe005, 0xf8ff, 0};

    Path iconFontPath = std::filesystem::current_path() / "assets" / "fonts" /
                        "FontAwesome-Solid.ttf";
    io.Fonts->AddFontFromFileTTF(iconFontPath.string().c_str(), FontSize,
                                 &config, IconRanges.data());
  }
}

void Theme::apply() {
  addFonts();
  setImguiStyles();
}

glm::vec4 Theme::getEngineColor(ThemeColor color) {
  auto c = SrgbToLinear(Colors.at(color));
  return glm::vec4(c.x, c.y, c.z, c.w);
}

glm::vec4 Theme::getClearColor() {
  return getEngineColor(ThemeColor::Charcoal800);
}

ImVec4 Theme::getColor(ThemeColor color) { return Colors.at(color); }

const ThemeStyles &Theme::getStyles() { return Styles; }

ImFont *Theme::getBoldFont() { return Fonts.at(1); }

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

} // namespace quoll::editor
