#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/imgui/Imgui.h"

#include "Theme.h"

namespace liquid::editor {

static constexpr float Gamma = 2.2f;

/**
 * @brief Srgb color to linear
 *
 * This function automatically converts
 * gamma corrected nonlinear colors to linear
 * ones; so, it is easier to apply colors
 * from graphics design applications to here
 * without manually performing calculations.
 *
 * @param r Red
 * @param g Greed
 * @param b Blue
 * @param a Alpha
 * @return Linear color
 */
static ImVec4 SrgbToLinear(int r, int g, int b, int a) {
  ImVec4 color(ImColor(r, g, b, a));

  return {std::pow(color.x, Gamma), std::pow(color.y, Gamma),
          std::pow(color.z, Gamma), color.w};
}

/**
 * @brief Srgb color to linear
 *
 * This function automatically converts
 * gamma corrected nonlinear colors to linear
 * ones; so, it is easier to apply colors
 * from graphics design applications to here
 * without manually performing calculations.
 *
 * @param r Red
 * @param g Greed
 * @param b Blue
 * @param a Alpha
 * @return Linear color
 */
static glm::vec4 SrgbToLinear(float r, float g, float b, float a) {
  return {std::pow(r, Gamma), std::pow(g, Gamma), std::pow(b, Gamma), a};
}

static constexpr float FontSize = 18.0f;

static constexpr size_t NumFonts = 2;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static std::array<ImFont *, NumFonts> Fonts{};

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
static const std::unordered_map<ThemeColor, glm::vec4> Colors{
    {ThemeColor::BackgroundColor, SrgbToLinear(0.22f, 0.22f, 0.22f, 1.0f)},
    {ThemeColor::SceneBackgroundColor,
     SrgbToLinear(0.46f, 0.60f, 0.70f, 1.0f)}};

static const std::unordered_map<ThemeStyle, glm::vec2> Styles{
    {ThemeStyle::SectionRounding, glm::vec2(8.0f, 8.0f)},
};
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

/**
 * @brief Set imgui styles
 */
static void setImguiStyles() {
  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
  auto &style = ImGui::GetStyle();

  // All items
  style.Colors[ImGuiCol_FrameBg] = SrgbToLinear(58, 58, 58, 255);
  style.Colors[ImGuiCol_FrameBgHovered] = SrgbToLinear(33, 33, 33, 255);
  style.Colors[ImGuiCol_FrameBgActive] = SrgbToLinear(33, 33, 33, 255);
  style.Colors[ImGuiCol_PopupBg] = SrgbToLinear(37, 37, 37, 255);
  style.Colors[ImGuiCol_ChildBg] = SrgbToLinear(37, 37, 37, 255);
  style.ItemSpacing = ImVec2(8.0f, 8.0f);
  style.ItemInnerSpacing = ImVec2(8.0f, 8.0f);
  style.FrameRounding = 4.0f;
  style.FramePadding = ImVec2(10.0f, 10.0f);
  style.FrameBorderSize = 0.0f;
  style.PopupBorderSize = 0.0f;
  style.PopupRounding = 4.0f;
  style.ChildRounding = 4.0f;

  // Tables
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = SrgbToLinear(68, 68, 68, 255);
  style.Colors[ImGuiCol_TableRowBg] = SrgbToLinear(68, 68, 68, 255);
  style.Colors[ImGuiCol_TableRowBgAlt] = SrgbToLinear(58, 58, 58, 255);
  style.CellPadding = ImVec2(8.0f, 8.0f);

  // Window
  style.Colors[ImGuiCol_WindowBg] = SrgbToLinear(44, 44, 44, 255);
  style.Colors[ImGuiCol_MenuBarBg] = SrgbToLinear(55, 55, 55, 255);
  style.Colors[ImGuiCol_TitleBg] = SrgbToLinear(68, 68, 68, 255);
  style.Colors[ImGuiCol_TitleBgCollapsed] = SrgbToLinear(68, 68, 68, 255);
  style.Colors[ImGuiCol_TitleBgActive] = SrgbToLinear(24, 24, 24, 255);
  style.Colors[ImGuiCol_ResizeGrip] = SrgbToLinear(255, 255, 255, 125);
  style.Colors[ImGuiCol_ResizeGripHovered] = SrgbToLinear(255, 255, 255, 255);
  style.Colors[ImGuiCol_ResizeGripActive] = SrgbToLinear(255, 255, 255, 255);
  style.WindowRounding = 8.0f;
  style.WindowPadding = ImVec2(10.0f, 10.0f);
  style.WindowBorderSize = 0.0f;

  // Tabs
  style.Colors[ImGuiCol_Tab] = SrgbToLinear(55, 55, 55, 255);
  style.Colors[ImGuiCol_TabUnfocused] = SrgbToLinear(55, 55, 55, 255);
  style.Colors[ImGuiCol_TabHovered] = SrgbToLinear(44, 44, 44, 255);
  style.Colors[ImGuiCol_TabActive] = SrgbToLinear(44, 44, 44, 255);
  style.Colors[ImGuiCol_TabUnfocusedActive] = SrgbToLinear(44, 44, 44, 255);

  // Headers
  // Used by collapsing header
  style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_HeaderActive] = SrgbToLinear(37, 37, 37, 255);
  style.Colors[ImGuiCol_HeaderHovered] = SrgbToLinear(37, 37, 37, 255);

  // Inputs
  style.Colors[ImGuiCol_Button] = SrgbToLinear(58, 58, 58, 255);
  style.Colors[ImGuiCol_ButtonHovered] = SrgbToLinear(33, 33, 33, 255);
  style.Colors[ImGuiCol_ButtonActive] = SrgbToLinear(33, 33, 33, 255);

  style.Colors[ImGuiCol_CheckMark] = SrgbToLinear(255, 255, 255, 255);
  style.Colors[ImGuiCol_SliderGrab] = SrgbToLinear(58, 58, 58, 255);
  style.Colors[ImGuiCol_SliderGrabActive] = SrgbToLinear(68, 68, 68, 255);
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
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

glm::vec4 Theme::getColor(ThemeColor color) { return Colors.at(color); }

glm::vec2 Theme::getStyle(ThemeStyle style) { return Styles.at(style); }

ImFont *Theme::getBoldFont() { return Fonts.at(1); }

} // namespace liquid::editor
