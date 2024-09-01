#include "quoll/core/Base.h"
#include "quoll/renderer/RendererAssetRegistry.h"
#include "ImguiUtils.h"

constexpr ImVec2 operator+(const ImVec2 &a, const ImVec2 &b) {
  return ImVec2(a.x + b.x, a.y + b.y);
}

constexpr ImVec2 operator-(const ImVec2 &a, const ImVec2 &b) {
  return ImVec2(a.x - b.x, a.y - b.y);
}

constexpr ImVec2 &operator+=(ImVec2 &a, const ImVec2 &b) {
  a.x += b.x;
  a.y += b.y;
  return a;
}

namespace quoll::imgui {

static RendererAssetRegistry *ImguiAssetRegistry = nullptr;

inline ImTextureID getImguiTexture(quoll::rhi::TextureHandle handle) {
  return reinterpret_cast<ImTextureID>(static_cast<uptr>(handle));
}

void setImguiAssetRegistry(RendererAssetRegistry &rendererAssetRegistry) {
  ImguiAssetRegistry = &rendererAssetRegistry;
}

void image(const AssetRef<TextureAsset> &asset, const ImVec2 &size,
           const ImVec2 &uv0, const ImVec2 &uv1, ImGuiID id,
           const ImVec4 &tint_col, const ImVec4 &border_col) {
  QuollAssert(ImguiAssetRegistry != nullptr,
              "Asset registry is null. Make sure to call "
              "`imgui::setImguiAssetRegistry`");

  image(ImguiAssetRegistry->get(asset), size, uv0, uv1, id, tint_col,
        border_col);
}

bool imageButton(const AssetRef<TextureAsset> &asset, const ImVec2 &size,
                 const ImVec2 &uv0, const ImVec2 &uv1, int frame_padding,
                 const ImVec4 &bg_col, const ImVec4 &tint_col) {
  QuollAssert(ImguiAssetRegistry != nullptr,
              "Asset registry is null. Make sure to call "
              "`imgui::setImguiAssetRegistry`");

  return imageButton(ImguiAssetRegistry->get(asset), size, uv0, uv1,
                     frame_padding, bg_col, tint_col);
}

void image(quoll::rhi::TextureHandle handle, const ImVec2 &size,
           const ImVec2 &uv0, const ImVec2 &uv1, ImGuiID id,
           const ImVec4 &tint_col, const ImVec4 &border_col) {

  static constexpr f32 BorderWidth = 2.0f;

  ImGuiWindow *window = ImGui::GetCurrentWindow();
  if (window->SkipItems)
    return;

  ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
  if (border_col.w > 0.0f) {
    bb.Max += ImVec2(BorderWidth, BorderWidth);
  }

  ImGui::ItemSize(bb);
  if (!ImGui::ItemAdd(bb, id))
    return;

  if (border_col.w > 0.0f) {
    window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(border_col),
                              0.0f);
    window->DrawList->AddImage(getImguiTexture(handle), bb.Min + ImVec2(1, 1),
                               bb.Max - ImVec2(1, 1), uv0, uv1,
                               ImGui::GetColorU32(tint_col));
  } else {
    window->DrawList->AddImage(getImguiTexture(handle), bb.Min, bb.Max, uv0,
                               uv1, ImGui::GetColorU32(tint_col));
  }
}

bool imageButton(quoll::rhi::TextureHandle handle, const ImVec2 &size,
                 const ImVec2 &uv0, const ImVec2 &uv1, int frame_padding,
                 const ImVec4 &bg_col, const ImVec4 &tint_col) {
  return ImGui::ImageButton(getImguiTexture(handle), size, uv0, uv1,
                            frame_padding, bg_col, tint_col);
}

} // namespace quoll::imgui
