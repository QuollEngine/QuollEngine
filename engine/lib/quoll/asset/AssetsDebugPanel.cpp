#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/imgui/ImguiUtils.h"
#include "AssetsDebugPanel.h"

namespace quoll::debug {

namespace {

template <typename TAssetData> void renderAssetMap(AssetCache *cache) {
  auto type = cache->getAssetType<TAssetData>();

  if (ImGui::BeginTabItem(getAssetTypeString(type).c_str())) {
    const auto &map = cache->getRegistry().getMap<TAssetData>();

    if (ImGui::BeginTable("Table", 4,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {

      // Header
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("UUID");

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Name");

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("Has loaded");

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Ref count");
      }

      for (auto &[handle, asset] : map.getMetas()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", asset.uuid.toString().c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", asset.name.c_str());

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", map.hasData(handle) ? "Yes" : "No");

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", map.getRefCount(handle));
      }

      ImGui::EndTable();
    }

    ImGui::EndTabItem();
  }
}

} // namespace

void AssetsDebugPanel::onRenderMenu() {
  ImGui::MenuItem("Assets", nullptr, &mOpen);
}

void AssetsDebugPanel::onRender() {
  if (!mOpen)
    return;

  if (ImGui::Begin("Assets", &mOpen, ImGuiWindowFlags_NoDocking)) {
    if (ImGui::BeginTabBar("Assets")) {
      renderAssetMap<TextureAsset>(mAssetCache);
      renderAssetMap<MaterialAsset>(mAssetCache);
      renderAssetMap<MeshAsset>(mAssetCache);
      renderAssetMap<SkeletonAsset>(mAssetCache);
      renderAssetMap<AnimationAsset>(mAssetCache);
      renderAssetMap<AnimatorAsset>(mAssetCache);
      renderAssetMap<AudioAsset>(mAssetCache);
      renderAssetMap<LuaScriptAsset>(mAssetCache);
      renderAssetMap<InputMapAsset>(mAssetCache);
      renderAssetMap<FontAsset>(mAssetCache);
      renderAssetMap<EnvironmentAsset>(mAssetCache);
      renderAssetMap<PrefabAsset>(mAssetCache);
      renderAssetMap<SceneAsset>(mAssetCache);

      ImGui::EndTabBar();
    }

    ImGui::End();
  }
}

} // namespace quoll::debug
