#pragma once

#include <imgui.h>
#include "liquid/rhi/RenderHandle.h"

namespace liquid::imgui {

void image(liquid::rhi::TextureHandle handle, const ImVec2 &size,
           const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1),
           const ImVec4 &tint_col = ImVec4(1, 1, 1, 1),
           const ImVec4 &border_col = ImVec4(0, 0, 0, 0));

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
