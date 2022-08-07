#include "liquid/core/Base.h"
#include "ImguiUtils.h"

namespace liquid::imgui {

inline ImTextureID getImguiTexture(liquid::rhi::TextureHandle handle) {
  return reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(handle));
}

void image(liquid::rhi::TextureHandle handle, const ImVec2 &size,
           const ImVec2 &uv0, const ImVec2 &uv1, const ImVec4 &tint_col,
           const ImVec4 &border_col) {
  ImGui::Image(getImguiTexture(handle), size, uv0, uv1, tint_col, border_col);
}

bool imageButton(liquid::rhi::TextureHandle handle, const ImVec2 &size,
                 const ImVec2 &uv0, const ImVec2 &uv1, int frame_padding,
                 const ImVec4 &bg_col, const ImVec4 &tint_col) {
  return ImGui::ImageButton(getImguiTexture(handle), size, uv0, uv1,
                            frame_padding, bg_col, tint_col);
}

bool input(const char *label, float &value, const char *format,
           ImGuiInputTextFlags flags) {
  ImGui::InputScalarN(label, ImGuiDataType_Float, &value, 1, NULL, NULL, format,
                      flags);

  return ImGui::IsItemDeactivatedAfterEdit();
}

bool input(const char *label, glm::vec3 &value, const char *format,
           ImGuiInputTextFlags flags) {
  ImGui::InputScalarN(label, ImGuiDataType_Float, glm::value_ptr(value), 3,
                      NULL, NULL, format, flags);

  return ImGui::IsItemDeactivatedAfterEdit();
}

bool inputColor(const char *label, glm::vec4 &value,
                ImGuiInputTextFlags flags) {
  ImGui::ColorEdit4(label, glm::value_ptr(value), flags);

  return ImGui::IsItemDeactivatedAfterEdit();
}

} // namespace liquid::imgui
