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

/**
 * @brief Srgb color to linear
 *
 * @param color Srgb color
 * @return Linear color
 */
static ImVec4 SrgbToLinear(ImColor color) {
  return {SrgbToLinear(color.Value.x), SrgbToLinear(color.Value.y),
          SrgbToLinear(color.Value.z), color.Value.w};
}

static constexpr f32 FontSize = 18.0f;

static constexpr usize NumFonts = 2;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static std::array<ImFont *, NumFonts> Fonts{};

static const std::unordered_map<ThemeColor, ImColor> Colors{
    {ThemeColor::White, ImColor(255, 255, 255)},
    {ThemeColor::Black, ImColor(0, 0, 0)},
    {ThemeColor::Transparent, ImColor(0, 0, 0, 0)},

    // Neutral colors
    {ThemeColor::Neutral100, ImColor(29, 29, 29)},
    {ThemeColor::Neutral200, ImColor(32, 32, 32)},
    {ThemeColor::Neutral300, ImColor(38, 38, 38)},
    {ThemeColor::Neutral400, ImColor(49, 49, 49)},
    {ThemeColor::Neutral500, ImColor(50, 50, 50)},
    {ThemeColor::Neutral600, ImColor(58, 58, 58)},
    {ThemeColor::Neutral700, ImColor(76, 76, 76)},
    {ThemeColor::Neutral800, ImColor(83, 83, 83)},
    {ThemeColor::Neutral900, ImColor(116, 116, 116)},

    // Primary colors
    {ThemeColor::Primary100, ImColor(210, 103, 47)},
    {ThemeColor::Primary200, ImColor(65, 67, 129)},

    // Misc
    {ThemeColor::ModalBackdrop, ImColor(0, 0, 0, 220)},

    // Semantic colors
    {ThemeColor::MidnightBlack100, ImColor(27, 27, 29)},
    {ThemeColor::MidnightBlack200, ImColor(32, 32, 35)}

    // End
};

static constexpr ThemeStyles Styles{.itemSpacing = ImVec2(8.0f, 8.0f),
                                    .sectionRounding = ImVec2(8.0f, 8.0f),
                                    .windowPadding = ImVec2(10.0f, 10.0f),
                                    .framePadding = ImVec2(10.0f, 10.0f),
                                    .childRounding = 4.0f};

/**
 * @brief Set imgui styles
 */
static void setImguiStyles() {
  auto &style = ImGui::GetStyle();

  // Separator
  style.Colors[ImGuiCol_Separator] = Theme::getColor(ThemeColor::Neutral500);
  style.Colors[ImGuiCol_Border] = Theme::getColor(ThemeColor::Neutral500);
  style.Colors[ImGuiCol_SeparatorHovered] =
      Theme::getColor(ThemeColor::Primary100);
  style.Colors[ImGuiCol_SeparatorActive] = ImColor(65, 67, 129);
  style.Colors[ImGuiCol_DockingPreview] =
      Theme::getColor(ThemeColor::Primary100);

  // All items
  style.Colors[ImGuiCol_FrameBg] =
      Theme::getColor(ThemeColor::MidnightBlack100);
  style.Colors[ImGuiCol_FrameBgHovered] =
      Theme::getColor(ThemeColor::Neutral700);
  style.Colors[ImGuiCol_FrameBgActive] =
      Theme::getColor(ThemeColor::Neutral700);
  style.Colors[ImGuiCol_PopupBg] = Theme::getColor(ThemeColor::Neutral100);
  style.Colors[ImGuiCol_ChildBg] = Theme::getColor(ThemeColor::Neutral100);
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
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImColor(68, 68, 68, 255);
  style.Colors[ImGuiCol_TableRowBg] = ImColor(68, 68, 68, 255);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImColor(58, 58, 58, 255);
  style.CellPadding = ImVec2(8.0f, 8.0f);

  // Window
  style.Colors[ImGuiCol_WindowBg] =
      Theme::getColor(ThemeColor::MidnightBlack200);
  style.Colors[ImGuiCol_MenuBarBg] =
      Theme::getColor(ThemeColor::MidnightBlack200);
  style.Colors[ImGuiCol_TitleBg] =
      Theme::getColor(ThemeColor::MidnightBlack100);
  style.Colors[ImGuiCol_TitleBgCollapsed] =
      Theme::getColor(ThemeColor::MidnightBlack100);
  style.Colors[ImGuiCol_TitleBgActive] =
      Theme::getColor(ThemeColor::MidnightBlack100);
  style.Colors[ImGuiCol_ResizeGrip] = ImColor(255, 255, 255, 125);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(255, 255, 255, 255);
  style.Colors[ImGuiCol_ResizeGripActive] = ImColor(255, 255, 255, 255);
  style.WindowRounding = 8.0f;
  style.WindowPadding = Styles.windowPadding;
  style.WindowBorderSize = 0.0f;

  // Tabs
  style.Colors[ImGuiCol_Tab] = Theme::getColor(ThemeColor::MidnightBlack100);
  style.Colors[ImGuiCol_TabUnfocused] =
      Theme::getColor(ThemeColor::MidnightBlack100);
  style.Colors[ImGuiCol_TabHovered] =
      Theme::getColor(ThemeColor::MidnightBlack200);
  style.Colors[ImGuiCol_TabActive] =
      Theme::getColor(ThemeColor::MidnightBlack200);
  style.Colors[ImGuiCol_TabUnfocusedActive] =
      Theme::getColor(ThemeColor::MidnightBlack200);

  // Headers
  // Used by collapsing header
  style.Colors[ImGuiCol_Header] = Theme::getColor(ThemeColor::Transparent);
  style.Colors[ImGuiCol_HeaderActive] = Theme::getColor(ThemeColor::Primary100);
  style.Colors[ImGuiCol_HeaderHovered] =
      Theme::getColor(ThemeColor::Primary100);

  // Buttons
  style.Colors[ImGuiCol_Button] = Theme::getColor(ThemeColor::MidnightBlack100);
  style.Colors[ImGuiCol_ButtonHovered] =
      Theme::getColor(ThemeColor::Primary100);
  style.Colors[ImGuiCol_ButtonActive] = Theme::getColor(ThemeColor::Primary100);

  style.Colors[ImGuiCol_CheckMark] = Theme::getColor(ThemeColor::White);
  style.Colors[ImGuiCol_SliderGrab] = Theme::getColor(ThemeColor::Neutral600);
  style.Colors[ImGuiCol_SliderGrabActive] =
      Theme::getColor(ThemeColor::Neutral900);
}

/**
 * @brief Add fonts
 */
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
  return getEngineColor(ThemeColor::MidnightBlack100);
}

ImVec4 Theme::getColor(ThemeColor color) { return Colors.at(color); }

const ThemeStyles &Theme::getStyles() { return Styles; }

ImFont *Theme::getBoldFont() { return Fonts.at(1); }

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

} // namespace quoll::editor
