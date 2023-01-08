#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/imgui/Imgui.h"

#include "Theme.h"

namespace liquidator {

static constexpr float FontSize = 18.0f;

static constexpr size_t NumFonts = 2;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static std::array<ImFont *, NumFonts> Fonts{};

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
static const std::unordered_map<ThemeColor, glm::vec4> Colors{
    {ThemeColor::BackgroundColor, glm::vec4(0.22f, 0.22f, 0.22f, 1.0f)},
    {ThemeColor::SceneBackgroundColor, glm::vec4(0.46f, 0.60f, 0.70f, 1.0f)}};

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
  style.Colors[ImGuiCol_FrameBg] = ImVec4(ImColor(58, 58, 58, 255));
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(ImColor(33, 33, 33, 255));
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(ImColor(33, 33, 33, 255));
  style.Colors[ImGuiCol_PopupBg] = ImVec4(ImColor(37, 37, 37, 255));
  style.Colors[ImGuiCol_ChildBg] = ImVec4(ImColor(37, 37, 37, 255));
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
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(ImColor(68, 68, 68, 255));
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(ImColor(68, 68, 68, 255));
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(ImColor(58, 58, 58, 255));
  style.CellPadding = ImVec2(8.0f, 8.0f);

  // Window
  style.Colors[ImGuiCol_WindowBg] = ImVec4(ImColor(44, 44, 44, 255));
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(ImColor(55, 55, 55, 255));
  style.Colors[ImGuiCol_TitleBg] = ImVec4(ImColor(68, 68, 68, 255));
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(ImColor(68, 68, 68, 255));
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(ImColor(24, 24, 24, 255));
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(ImColor(255, 255, 255, 125));
  style.Colors[ImGuiCol_ResizeGripHovered] =
      ImVec4(ImColor(255, 255, 255, 255));
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(ImColor(255, 255, 255, 255));
  style.WindowRounding = 8.0f;
  style.WindowPadding = ImVec2(10.0f, 10.0f);
  style.WindowBorderSize = 0.0f;

  // Tabs
  style.Colors[ImGuiCol_Tab] = ImVec4(ImColor(55, 55, 55, 255));
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(ImColor(55, 55, 55, 255));
  style.Colors[ImGuiCol_TabHovered] = ImVec4(ImColor(44, 44, 44, 255));
  style.Colors[ImGuiCol_TabActive] = ImVec4(ImColor(44, 44, 44, 255));
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(ImColor(44, 44, 44, 255));

  // Headers
  // Used by collapsing header
  style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(ImColor(37, 37, 37, 255));
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(ImColor(37, 37, 37, 255));

  // Inputs
  style.Colors[ImGuiCol_Button] = ImVec4(ImColor(58, 58, 58, 255));
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(ImColor(33, 33, 33, 255));
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(ImColor(33, 33, 33, 255));

  style.Colors[ImGuiCol_CheckMark] = ImVec4(ImColor(255, 255, 255, 255));
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(ImColor(58, 58, 58, 255));
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(ImColor(68, 68, 68, 255));
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}

/**
 * @brief Add fonts
 */
static void addFonts() {
  auto &io = ImGui::GetIO();

  liquid::Path defaultFontPath =
      liquid::Engine::getFontsPath() / "Roboto-Regular.ttf";
  Fonts.at(0) =
      io.Fonts->AddFontFromFileTTF(defaultFontPath.string().c_str(), FontSize);

  liquid::Path boldFontPath =
      std::filesystem::current_path() / "assets" / "fonts" / "Roboto-Bold.ttf";
  Fonts.at(1) =
      io.Fonts->AddFontFromFileTTF(boldFontPath.string().c_str(), FontSize);

  for (size_t i = 0; i < Fonts.size(); ++i) {
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = FontSize;
    config.DstFont = Fonts.at(i);

    static constexpr std::array<ImWchar, 3> IconRanges{0xe005, 0xf8ff, 0};

    liquid::Path iconFontPath = std::filesystem::current_path() / "assets" /
                                "fonts" / "FontAwesome-Solid.ttf";
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

} // namespace liquidator
