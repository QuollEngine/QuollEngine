#include "liquid/core/Base.h"
#include "EnvironmentPanel.h"

#include "liquid/imgui/ImguiUtils.h"
#include "Widgets.h"

namespace liquid::editor {

void EnvironmentPanel::render(EditorManager &editorManager,
                              AssetManager &assetManager) {
  static constexpr float HALF = 0.5f;

  auto &textures = assetManager.getAssetRegistry().getTextures();

  if (auto _ = widgets::Window("Environment")) {
    float width = ImGui::GetWindowWidth();
    float height = width * HALF;

    if (rhi::isHandleValid(editorManager.getEnvironment().irradianceMap)) {
      ImGui::Button("Set irradiance (texture set)", ImVec2(width, height));
    } else {
      ImGui::Button("Set irradiance", ImVec2(width, height));
    }

    if (ImGui::BeginDragDropTarget()) {
      if (auto *payload = ImGui::AcceptDragDropPayload(
              getAssetTypeString(AssetType::Texture).c_str())) {
        auto handle = *static_cast<TextureAssetHandle *>(payload->Data);
        auto &asset = textures.getAsset(handle);

        editorManager.getEnvironment().irradianceMap = asset.data.deviceHandle;
      }
    }

    if (rhi::isHandleValid(editorManager.getEnvironment().specularMap)) {
      ImGui::Button("Set specular (texture set)", ImVec2(width, height));
    } else {
      ImGui::Button("Set specular", ImVec2(width, height));
    }

    if (ImGui::BeginDragDropTarget()) {
      if (auto *payload = ImGui::AcceptDragDropPayload(
              getAssetTypeString(AssetType::Texture).c_str())) {
        auto handle = *static_cast<TextureAssetHandle *>(payload->Data);
        auto &asset = textures.getAsset(handle);

        editorManager.getEnvironment().specularMap = asset.data.deviceHandle;
      }
    }

    if (rhi::isHandleValid(editorManager.getEnvironment().brdfLUT)) {
      imgui::imageButton(editorManager.getEnvironment().brdfLUT,
                         ImVec2(width, height));
    } else {
      ImGui::Button("Set BRDF", ImVec2(width, height));
    }

    if (ImGui::BeginDragDropTarget()) {
      if (auto *payload = ImGui::AcceptDragDropPayload(
              getAssetTypeString(AssetType::Texture).c_str())) {
        auto handle = *static_cast<TextureAssetHandle *>(payload->Data);
        auto &asset = textures.getAsset(handle);

        editorManager.getEnvironment().brdfLUT = asset.data.deviceHandle;
      }
    }
  }
}

} // namespace liquid::editor
