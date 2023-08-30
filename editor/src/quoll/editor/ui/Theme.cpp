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
static constexpr float SrgbToLinear(float value) {
  if (value <= 0.0031308f) {
    return value / 12.92f;
  }

  return pow((value + 0.055f) / 1.055f, 2.4f);
}

/**
 * @brief Srgb color to linear
 *
 * @param r Red
 * @param g Greed
 * @param b Blue
 * @param a Alpha
 * @return Linear color
 */
static ImVec4 SrgbToLinear(int r, int g, int b, int a = 255) {
  ImVec4 color(ImColor(r, g, b, a));

  return {SrgbToLinear(color.x), SrgbToLinear(color.y), SrgbToLinear(color.z),
          color.w};
}

static constexpr float FontSize = 18.0f;

static constexpr size_t NumFonts = 2;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static std::array<ImFont *, NumFonts> Fonts{};

static const std::unordered_map<ThemeColor, ImVec4> Colors{
    {ThemeColor::White, SrgbToLinear(255, 255, 255)},
    {ThemeColor::Black, SrgbToLinear(0, 0, 0)},
    {ThemeColor::Transparent, SrgbToLinear(0, 0, 0, 0)},

    // Neutral colors
    {ThemeColor::Neutral100, SrgbToLinear(29, 29, 29)},
    {ThemeColor::Neutral200, SrgbToLinear(32, 32, 32)},
    {ThemeColor::Neutral300, SrgbToLinear(38, 38, 38)},
    {ThemeColor::Neutral400, SrgbToLinear(49, 49, 49)},
    {ThemeColor::Neutral500, SrgbToLinear(50, 50, 50)},
    {ThemeColor::Neutral600, SrgbToLinear(58, 58, 58)},
    {ThemeColor::Neutral700, SrgbToLinear(76, 76, 76)},
    {ThemeColor::Neutral800, SrgbToLinear(83, 83, 83)},
    {ThemeColor::Neutral900, SrgbToLinear(116, 116, 116)},

    // Primary colors
    {ThemeColor::Primary100, SrgbToLinear(52, 55, 110)},
    {ThemeColor::Primary200, SrgbToLinear(65, 67, 129)},

    // Misc
    {ThemeColor::ModalBackdrop, SrgbToLinear(0, 0, 0, 220)},

    // End
};

static constexpr ThemeStyles Styles{.itemSpacing = ImVec2(8.0f, 8.0f),
                                    .sectionRounding = ImVec2(8.0f, 8.0f),
                                    .windowPadding = ImVec2(10.0f, 10.0f),
                                    .childRounding = 4.0f};

/**
 * @brief Set imgui styles
 */
static void setImguiStyles() {
  auto &style = ImGui::GetStyle();

  // Separator
  style.Colors[ImGuiCol_Separator] = Theme::getColor(ThemeColor::Neutral500);
  style.Colors[ImGuiCol_SeparatorHovered] =
      Theme::getColor(ThemeColor::Primary100);
  style.Colors[ImGuiCol_SeparatorActive] = SrgbToLinear(65, 67, 129);
  style.Colors[ImGuiCol_DockingPreview] =
      Theme::getColor(ThemeColor::Primary100);

  // All items
  style.Colors[ImGuiCol_FrameBg] = Theme::getColor(ThemeColor::Neutral800);
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
  style.FramePadding = ImVec2(10.0f, 10.0f);
  style.FrameBorderSize = 0.0f;
  style.PopupBorderSize = 0.0f;
  style.PopupRounding = 4.0f;
  style.ChildRounding = Styles.childRounding;
  style.WindowMenuButtonPosition = ImGuiDir_None;

  // Tables
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = SrgbToLinear(68, 68, 68, 255);
  style.Colors[ImGuiCol_TableRowBg] = SrgbToLinear(68, 68, 68, 255);
  style.Colors[ImGuiCol_TableRowBgAlt] = SrgbToLinear(58, 58, 58, 255);
  style.CellPadding = ImVec2(8.0f, 8.0f);

  // Window
  style.Colors[ImGuiCol_WindowBg] = Theme::getColor(ThemeColor::Neutral300);
  style.Colors[ImGuiCol_MenuBarBg] = Theme::getColor(ThemeColor::Neutral300);
  style.Colors[ImGuiCol_TitleBg] = Theme::getColor(ThemeColor::Neutral300);
  style.Colors[ImGuiCol_TitleBgCollapsed] =
      Theme::getColor(ThemeColor::Neutral300);
  style.Colors[ImGuiCol_TitleBgActive] =
      Theme::getColor(ThemeColor::Neutral300);
  style.Colors[ImGuiCol_ResizeGrip] = SrgbToLinear(255, 255, 255, 125);
  style.Colors[ImGuiCol_ResizeGripHovered] = SrgbToLinear(255, 255, 255, 255);
  style.Colors[ImGuiCol_ResizeGripActive] = SrgbToLinear(255, 255, 255, 255);
  style.WindowRounding = 8.0f;
  style.WindowPadding = Styles.windowPadding;
  style.WindowBorderSize = 0.0f;

  // Tabs
  style.Colors[ImGuiCol_Tab] = Theme::getColor(ThemeColor::Neutral300);
  style.Colors[ImGuiCol_TabUnfocused] = Theme::getColor(ThemeColor::Neutral300);
  style.Colors[ImGuiCol_TabHovered] = Theme::getColor(ThemeColor::Neutral500);
  style.Colors[ImGuiCol_TabActive] = Theme::getColor(ThemeColor::Neutral500);
  style.Colors[ImGuiCol_TabUnfocusedActive] =
      Theme::getColor(ThemeColor::Neutral500);

  // Headers
  // Used by collapsing header
  style.Colors[ImGuiCol_Header] = Theme::getColor(ThemeColor::Transparent);
  style.Colors[ImGuiCol_HeaderActive] = Theme::getColor(ThemeColor::Primary100);
  style.Colors[ImGuiCol_HeaderHovered] =
      Theme::getColor(ThemeColor::Primary100);

  // Buttons
  style.Colors[ImGuiCol_Button] = Theme::getColor(ThemeColor::Neutral500);
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

  for (size_t i = 0; i < Fonts.size(); ++i) {
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
  auto c = Colors.at(color);
  return glm::vec4(c.x, c.y, c.z, c.w);
}

glm::vec4 Theme::getClearColor() {
  return getEngineColor(ThemeColor::Neutral500);
}

ImVec4 Theme::getColor(ThemeColor color) { return Colors.at(color); }

const ThemeStyles &Theme::getStyles() { return Styles; }

ImFont *Theme::getBoldFont() { return Fonts.at(1); }

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

} // namespace quoll::editor
