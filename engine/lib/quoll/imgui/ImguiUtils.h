#pragma once

#include "quoll/asset/AssetRef.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/renderer/TextureAsset.h"
#include "quoll/rhi/RenderHandle.h"

namespace quoll {

class RendererAssetRegistry;

}

namespace quoll::imgui {

void setImguiAssetRegistry(RendererAssetRegistry &rendererAssetRegistry);

void image(const AssetRef<TextureAsset> &asset, const ImVec2 &size,
           const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1),
           ImGuiID id = 0, const ImVec4 &tint_col = ImVec4(1, 1, 1, 1),
           const ImVec4 &border_col = ImVec4(0, 0, 0, 0));

bool imageButton(const AssetRef<TextureAsset> &asset, const ImVec2 &size,
                 const ImVec2 &uv0 = ImVec2(0, 0),
                 const ImVec2 &uv1 = ImVec2(1, 1), int frame_padding = -1,
                 const ImVec4 &bg_col = ImVec4(0, 0, 0, 0),
                 const ImVec4 &tint_col = ImVec4(1, 1, 1, 1));

void image(quoll::rhi::TextureHandle handle, const ImVec2 &size,
           const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1),
           ImGuiID id = 0, const ImVec4 &tint_col = ImVec4(1, 1, 1, 1),
           const ImVec4 &border_col = ImVec4(0, 0, 0, 0));

bool imageButton(quoll::rhi::TextureHandle handle, const ImVec2 &size,
                 const ImVec2 &uv0 = ImVec2(0, 0),
                 const ImVec2 &uv1 = ImVec2(1, 1), int frame_padding = -1,
                 const ImVec4 &bg_col = ImVec4(0, 0, 0, 0),
                 const ImVec4 &tint_col = ImVec4(1, 1, 1, 1));

} // namespace quoll::imgui
