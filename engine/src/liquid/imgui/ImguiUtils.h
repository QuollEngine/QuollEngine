#pragma once

#include <imgui.h>
#include "liquid/rhi/RenderHandle.h"

namespace liquid::imgui {

void image(liquid::rhi::TextureHandle handle, const ImVec2 &size,
           const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1),
           const ImVec4 &tint_col = ImVec4(1, 1, 1, 1),
           const ImVec4 &border_col = ImVec4(0, 0, 0, 0));

bool imageButton(liquid::rhi::TextureHandle handle, const ImVec2 &size,
                 const ImVec2 &uv0 = ImVec2(0, 0),
                 const ImVec2 &uv1 = ImVec2(1, 1), int frame_padding = -1,
                 const ImVec4 &bg_col = ImVec4(0, 0, 0, 0),
                 const ImVec4 &tint_col = ImVec4(1, 1, 1, 1));

bool input(const char *label, float &value, const char *format = "%0.3f",
           ImGuiInputTextFlags flags = 0);

bool input(const char *label, glm::vec3 &value, const char *format = "%0.3f",
           ImGuiInputTextFlags flags = 0);

bool inputColor(const char *label, glm::vec4 &value,
                ImGuiInputTextFlags flags = 0);

void renderColumn(const glm::vec3 &value);

void renderColumn(const glm::quat &value);

void renderColumn(float value);

void renderColumn(const String &value);

void renderColumn(uint32_t value);

template <class... Args> void renderRow(const Args &...args) {
  ImGui::TableNextRow();
  (renderColumn(args), ...);
}

} // namespace liquid::imgui
