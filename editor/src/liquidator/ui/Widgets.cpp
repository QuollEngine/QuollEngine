#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "Widgets.h"
#include "Theme.h"

namespace liquidator::widgets {

/**
 * @brief Section horizontal bounds
 */
struct XBounds {
  /**
   * Start bound
   */
  float start = 0.0f;

  /**
   * End bound
   */
  float end = 0.0f;
};

/**
 * @brief Calculate section horizontal bounds
 *
 * @param padding Horizontal padding
 * @return Horizontal bounds
 */
static XBounds calculateSectionBoundsX(float padding) {
  auto *window = ImGui::GetCurrentWindow();
  float windowStart = ImGui::GetWindowPos().x;

  return {windowStart + window->WindowPadding.x + padding,
          windowStart + ImGui::GetWindowWidth() - window->WindowPadding.x -
              padding};
}

bool Section::begin(const char *title) {
  ImGui::GetWindowDrawList()->ChannelsSplit(2);
  ImGui::GetWindowDrawList()->ChannelsSetCurrent(1);

  auto padding = ImGui::GetStyle().WindowPadding;

  auto *window = ImGui::GetCurrentWindow();
  float windowWidth = ImGui::GetWindowWidth();

  auto boundsX = calculateSectionBoundsX(padding.x);

  const float midPoint = boundsX.start + (boundsX.end - boundsX.start) / 2.0f;

  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding.y);
  ImGui::BeginGroup();
  if (padding.x > 0) {
    ImGui::Indent(padding.x);
  }

  ImGui::PushClipRect(ImVec2(boundsX.start, window->ClipRect.Min.y),
                      ImVec2(midPoint, window->ClipRect.Max.y), false);
  ImGui::Text("%s", title);
  ImGui::PopClipRect();

  ImGui::PushClipRect(ImVec2(boundsX.start, window->ClipRect.Min.y),
                      ImVec2(boundsX.end, window->ClipRect.Max.y), false);

  return true;
}

void Section::end() {
  auto padding = ImGui::GetStyle().WindowPadding;

  ImGui::PopClipRect();
  if (padding.x > 0) {
    ImGui::Unindent(padding.x);
  }
  ImGui::EndGroup();

  auto boundsX = calculateSectionBoundsX(0.0f);

  auto panelMin = ImVec2(boundsX.start, ImGui::GetItemRectMin().y - padding.y);
  auto panelMax = ImVec2(boundsX.end, ImGui::GetItemRectMax().y + padding.y);

  ImGui::GetWindowDrawList()->ChannelsSetCurrent(0);
  ImGui::GetWindowDrawList()->AddRectFilled(
      panelMin, panelMax,
      ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ChildBg)),
      Theme::getStyle(ThemeStyle::SectionRounding).x);
  ImGui::GetWindowDrawList()->ChannelsMerge();

  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding.y);
  ImGui::Spacing();
}

bool Window::begin(const char *title) { return ImGui::Begin(title, nullptr); }

void Window::end() { ImGui::End(); }

bool MainMenuBar::begin() {
  bool begin = ImGui::BeginMainMenuBar();

  if (begin) {
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                          ImGui::GetStyleColorVec4(ImGuiCol_TitleBgCollapsed));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                          ImGui::GetStyleColorVec4(ImGuiCol_TitleBgCollapsed));
  }

  return begin;
}

void MainMenuBar::end() {
  ImGui::PopStyleColor(2);
  ImGui::EndMainMenuBar();
}

bool ContextMenu::begin() {
  bool begin = ImGui::BeginPopupContextWindow();

  if (begin) {
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                          ImGui::GetStyleColorVec4(ImGuiCol_TitleBgCollapsed));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                          ImGui::GetStyleColorVec4(ImGuiCol_TitleBgCollapsed));
  }

  return begin;
}

void ContextMenu::end() {
  ImGui::PopStyleColor(2);
  ImGui::EndPopup();
}

} // namespace liquidator::widgets
