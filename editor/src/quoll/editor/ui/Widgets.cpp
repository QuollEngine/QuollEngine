#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/imgui/ImguiUtils.h"

#include "Widgets.h"
#include "Theme.h"

namespace quoll::editor::widgets {

bool Button(const char *label, ImVec2 size) {
  return ImGui::Button(label, size);
}

struct XBounds {
  f32 start = 0.0f;

  f32 end = 0.0f;
};

static XBounds calculateSectionBoundsX(f32 padding) {
  auto *window = ImGui::GetCurrentWindow();
  f32 windowStart = ImGui::GetWindowPos().x;

  return {windowStart + window->WindowPadding.x + padding,
          windowStart + ImGui::GetWindowWidth() - window->WindowPadding.x -
              padding};
}

Section::Section(const char *title) {
  ImGui::GetWindowDrawList()->ChannelsSplit(2);
  ImGui::GetWindowDrawList()->ChannelsSetCurrent(1);

  mPadding = ImGui::GetStyle().WindowPadding;

  auto *window = ImGui::GetCurrentWindow();
  f32 windowWidth = ImGui::GetWindowWidth();

  auto boundsX = calculateSectionBoundsX(mPadding.x);

  const f32 midPoint = boundsX.start + (boundsX.end - boundsX.start) / 1.2f;

  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + mPadding.y);
  ImGui::BeginGroup();
  if (mPadding.x > 0) {
    ImGui::Indent(mPadding.x);
  }

  ImGui::PushClipRect(ImVec2(boundsX.start, window->ClipRect.Min.y),
                      ImVec2(midPoint, window->ClipRect.Max.y), false);
  ImGui::Text("%s", title);
  ImGui::PopClipRect();

  mClipRect.Min = ImVec2(boundsX.start, window->ClipRect.Min.y);
  mClipRect.Max = ImVec2(boundsX.end, window->ClipRect.Max.y);
  ImGui::PushClipRect(mClipRect.Min, mClipRect.Max, false);

  mExpanded = true;
}

Section::~Section() {
  if (mExpanded) {
    auto padding = ImGui::GetStyle().WindowPadding;

    ImGui::PopClipRect();
    if (padding.x > 0) {
      ImGui::Unindent(padding.x);
    }

    auto boundsX = calculateSectionBoundsX(0.0f);
    auto *window = ImGui::GetCurrentWindow();
    // Hack: Needed to ensure that the group
    // takes up full width of the parent area
    window->DC.CursorMaxPos.x = boundsX.end;

    ImGui::EndGroup();

    auto panelMin =
        ImVec2(boundsX.start, ImGui::GetItemRectMin().y - padding.y);
    auto panelMax = ImVec2(boundsX.end, ImGui::GetItemRectMax().y + padding.y);

    ImGui::GetWindowDrawList()->ChannelsSetCurrent(0);
    ImGui::GetWindowDrawList()->AddRectFilled(
        panelMin, panelMax,
        ImGui::GetColorU32(Theme::getColor(ThemeColor::Charcoal400)),
        Theme::getStyles().sectionRounding.x);
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

ContextMenu::ContextMenu() { mExpanded = ImGui::BeginPopupContextWindow(); }

ContextMenu::~ContextMenu() {
  if (mExpanded) {
    ImGui::EndPopup();
  }
}

Table::Table(const char *id, u32 numColumns) {
  mExpanded = ImGui::BeginTable(id, static_cast<int>(numColumns),
                                ImGuiTableFlags_Borders |
                                    ImGuiTableFlags_SizingStretchSame |
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

void Table::column(const glm::vec2 &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f %.2f", value.x, value.y);
}

void Table::column(const glm::quat &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f %.2f %.2f %.2f", value.x, value.y, value.z, value.w);
}

void Table::column(f32 value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f", value);
}

void Table::column(const String &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%s", value.c_str());
}

void Table::column(i32 value) {
  ImGui::TableNextColumn();
  ImGui::Text("%d", value);
}

void Table::column(u32 value) {
  ImGui::TableNextColumn();
  ImGui::Text("%d", value);
}

void Table::column(rhi::TextureHandle handle, const glm::vec2 &size) {
  ImGui::TableNextColumn();
  imgui::image(handle, ImVec2(size.x, size.y));
}

Input::Input(String label, glm::vec3 &value, bool autoChange) {
  if (autoChange) {
    auto temp = value;

    renderScalarInput(label, glm::value_ptr(temp), glm::vec3::length(),
                      ImGuiDataType_Float);

    if (mChanged) {
      value = temp;
    }

  } else {
    renderScalarInput(label, glm::value_ptr(value), glm::vec3::length(),
                      ImGuiDataType_Float);
  }
}

Input::Input(String label, glm::vec2 &value, bool autoChange) {
  if (autoChange) {
    auto temp = value;

    renderScalarInput(label, glm::value_ptr(temp), glm::vec2::length(),
                      ImGuiDataType_Float);

    if (mChanged) {
      value = temp;
    }

  } else {
    renderScalarInput(label, glm::value_ptr(value), glm::vec2::length(),
                      ImGuiDataType_Float);
  }
}

Input::Input(String label, f32 &value, bool autoChange) {
  if (autoChange) {
    auto temp = value;

    renderScalarInput(label, &temp, 1, ImGuiDataType_Float);

    if (mChanged) {
      value = temp;
    }

  } else {
    renderScalarInput(label, &value, 1, ImGuiDataType_Float);
  }
}

Input::Input(String label, u32 &value, bool autoChange) {
  if (autoChange) {
    u32 temp = value;

    renderScalarInput(label, &temp, 1, ImGuiDataType_U32);

    if (mChanged) {
      value = temp;
    }
  } else {
    renderScalarInput(label, &value, 1, ImGuiDataType_U32);
  }
}

Input::Input(String label, String &value, bool autoChange) {
  if (autoChange) {
    auto temp = value;
    renderTextInput(label, temp);

    if (mChanged) {
      value = temp;
    }
  } else {
    renderTextInput(label, value);
  }
}

void Input::renderScalarInput(String label, void *data, usize size,
                              ImGuiDataType dataType) {
  if (!label.empty()) {
    ImGui::Text("%s", label.c_str());
  }
  const auto id = "###Input" + label;

  ImGui::InputScalarN(id.c_str(), dataType, data, static_cast<int>(size),
                      nullptr, nullptr,
                      dataType == ImGuiDataType_Float ? "%.3f" : 0);
  mChanged = ImGui::IsItemDeactivatedAfterEdit();
}

void Input::renderTextInput(String label, String &data) {
  if (!label.empty()) {
    ImGui::Text("%s", label.c_str());
  }
  const auto id = "###Input" + label;

  ImGui::InputText(
      id.c_str(), const_cast<char *>(data.c_str()), data.capacity() + 1,
      ImGuiInputTextFlags_CallbackResize,
      [](ImGuiInputTextCallbackData *data) -> int {
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
          String *str = static_cast<String *>(data->UserData);

          str->resize(data->BufTextLen);
          data->Buf = const_cast<char *>(str->c_str());
        }
        return 0;
      },
      &data);

  mChanged = ImGui::IsItemDeactivatedAfterEdit();
}

InputColor::InputColor(String label, glm::vec4 &value) {
  if (!label.empty()) {
    ImGui::Text("%s", label.c_str());
  }
  const auto id = "###Input" + label;

  mChanged = ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value));
}

} // namespace quoll::editor::widgets
