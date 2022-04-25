#include "liquid/core/Base.h"
#include "ImguiUtils.h"

namespace liquid::imgui {

void image(liquid::rhi::TextureHandle handle, const ImVec2 &size,
           const ImVec2 &uv0, const ImVec2 &uv1, const ImVec4 &tint_col,
           const ImVec4 &border_col) {
  ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(handle)), size,
               uv0, uv1, tint_col, border_col);
}

void renderColumn(const glm::vec3 &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f %.2f %.2f", value.x, value.y, value.z);
}

void renderColumn(const glm::quat &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f %.2f %.2f %.2f", value.x, value.y, value.z, value.w);
}

void renderColumn(float value) {
  ImGui::TableNextColumn();
  ImGui::Text("%.2f", value);
}

void renderColumn(const String &value) {
  ImGui::TableNextColumn();
  ImGui::Text("%s", value.c_str());
}

} // namespace liquid::imgui
