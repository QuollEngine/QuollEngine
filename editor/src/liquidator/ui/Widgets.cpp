#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"
#include "liquid/imgui/ImguiUtils.h"

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

Section::Section(const char *title) {
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

  mExpanded = true;
}

Section::~Section() {
  if (mExpanded) {
    auto padding = ImGui::GetStyle().WindowPadding;

    ImGui::PopClipRect();
    if (padding.x > 0) {
      ImGui::Unindent(padding.x);
    }
    ImGui::EndGroup();

    auto boundsX = calculateSectionBoundsX(0.0f);

    auto panelMin =
        ImVec2(boundsX.start, ImGui::GetItemRectMin().y - padding.y);
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
}

Window::Window(const char *title) { mExpanded = ImGui::Begin(title, nullptr); }

Window::~Window() { ImGui::End(); }

FixedWindow::FixedWindow(const char *title, bool &open) {
  mExpanded = ImGui::Begin(title, &open, ImGuiWindowFlags_NoDocking);
}

FixedWindow::~FixedWindow() { ImGui::End(); }

MainMenuBar::MainMenuBar() {
  mExpanded = ImGui::BeginMainMenuBar();

  if (mExpanded) {
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                          ImGui::GetStyleColorVec4(ImGuiCol_TitleBgCollapsed));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                          ImGui::GetStyleColorVec4(ImGuiCol_TitleBgCollapsed));
  }
}

MainMenuBar::~MainMenuBar() {
  if (mExpanded) {
    ImGui::PopStyleColor(2);
    ImGui::EndMainMenuBar();
  }
}

ContextMenu::ContextMenu() {
  mExpanded = ImGui::BeginPopupContextWindow();

  if (mExpanded) {
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                          ImGui::GetStyleColorVec4(ImGuiCol_TitleBgCollapsed));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                          ImGui::GetStyleColorVec4(ImGuiCol_TitleBgCollapsed));
  }
}

ContextMenu::~ContextMenu() {
  if (mExpanded) {
    ImGui::PopStyleColor(2);
    ImGui::EndPopup();
  }
}

Table::Table(const char *id, uint32_t numColumns) {
  mExpanded = ImGui::BeginTable(id, static_cast<int>(numColumns),
                                ImGuiTableFlags_Borders |
                                    ImGuiTableColumnFlags_WidthStretch |
                                    ImGuiTableFlags_RowBg);
}

Table::~Table() {
  if (mExpanded)
    ImGui::EndTable();
}

void Table::column(const glm::vec4 &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f %.2f %.2f %.2f", value.x, value.y, value.z, value.w);
}

void Table::column(const glm::vec3 &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f %.2f %.2f", value.x, value.y, value.z);
}

void Table::column(const glm::quat &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f %.2f %.2f %.2f", value.x, value.y, value.z, value.w);
}

void Table::column(float value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f", value);
}

void Table::column(const liquid::String &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%s", value.c_str());
}

void Table::column(int32_t value) {
  ImGui::TableNextColumn();
  ImGui::Text("%d", value);
}

void Table::column(uint32_t value) {
  ImGui::TableNextColumn();
  ImGui::Text("%d", value);
}

void Table::column(liquid::rhi::TextureHandle handle, const glm::vec2 &size) {
  ImGui::TableNextColumn();
  liquid::imgui::image(handle, ImVec2(size.x, size.y));
}

Input::Input(liquid::String label, glm::vec3 &value) {
  glm::vec3 temp = value;

  renderScalarInput(label, glm::value_ptr(temp), glm::vec3::length());

  if (mChanged) {
    value = temp;
  }
}

Input::Input(liquid::String label, float &value) {
  float temp = value;

  renderScalarInput(label, &temp, 1);

  if (mChanged) {
    value = temp;
  }
}

Input::Input(liquid::String label, liquid::String &value) {
  auto temp = value;
  renderTextInput(label, temp);

  if (mChanged) {
    value = temp;
  }
}

void Input::renderScalarInput(liquid::String label, void *data, size_t size) {
  if (!label.empty()) {
    ImGui::Text("%s", label.c_str());
  }
  const auto id = "###Input" + label;

  ImGui::InputScalarN(id.c_str(), ImGuiDataType_Float, data,
                      static_cast<int>(size), nullptr, nullptr, "%.3f");
  mChanged = ImGui::IsItemDeactivatedAfterEdit();
}

void Input::renderTextInput(liquid::String label, liquid::String &data) {
  if (!label.empty()) {
    ImGui::Text("%s", label.c_str());
  }
  const auto id = "###Input" + label;

  ImGui::InputText(
      id.c_str(), const_cast<char *>(data.c_str()), data.capacity() + 1,
      ImGuiInputTextFlags_CallbackResize,
      [](ImGuiInputTextCallbackData *data) -> int {
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
          liquid::String *str = static_cast<liquid::String *>(data->UserData);

          str->resize(data->BufTextLen);
          data->Buf = const_cast<char *>(str->c_str());
        }
        return 0;
      },
      &data);

  mChanged = ImGui::IsItemDeactivatedAfterEdit();
}

InputColor::InputColor(liquid::String label, glm::vec4 &value) {
  if (!label.empty()) {
    ImGui::Text("%s", label.c_str());
  }
  const auto id = "###Input" + label;

  mChanged = ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value));
}

} // namespace liquidator::widgets
