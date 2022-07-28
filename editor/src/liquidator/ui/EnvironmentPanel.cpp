#include "liquid/core/Base.h"
#include "EnvironmentPanel.h"

#include "liquid/imgui/ImguiUtils.h"

namespace liquidator {

void EnvironmentPanel::render(EditorManager &editorManager,
                              liquid::AssetManager &assetManager) {
  static constexpr float HALF = 0.5f;
  if (ImGui::BeginTabItem("Environment", 0)) {
    float width = ImGui::GetWindowWidth();
    float height = width * HALF;

    if (liquid::rhi::isHandleValid(
            editorManager.getEnvironment().irradianceMap)) {
      ImGui::Button("Set irradiance (texture set)", ImVec2(width, height));
    } else {
      ImGui::Button("Set irradiance", ImVec2(width, height));
    }

    if (ImGui::BeginDragDropTarget()) {
      if (auto *payload = ImGui::AcceptDragDropPayload(
              liquid::getAssetTypeString(liquid::AssetType::Texture).c_str())) {
        auto handle = *static_cast<liquid::TextureAssetHandle *>(payload->Data);
        auto &asset = assetManager.getRegistry().getTextures().getAsset(handle);

        editorManager.getEnvironment().irradianceMap = asset.data.deviceHandle;
      }
    }

    if (liquid::rhi::isHandleValid(
            editorManager.getEnvironment().specularMap)) {
      ImGui::Button("Set specular (texture set)", ImVec2(width, height));
    } else {
      ImGui::Button("Set specular", ImVec2(width, height));
    }

    if (ImGui::BeginDragDropTarget()) {
      if (auto *payload = ImGui::AcceptDragDropPayload(
              liquid::getAssetTypeString(liquid::AssetType::Texture).c_str())) {
        auto handle = *static_cast<liquid::TextureAssetHandle *>(payload->Data);
        auto &asset = assetManager.getRegistry().getTextures().getAsset(handle);

        editorManager.getEnvironment().specularMap = asset.data.deviceHandle;
      }
    }

    if (liquid::rhi::isHandleValid(editorManager.getEnvironment().brdfLUT)) {
      liquid::imgui::imageButton(editorManager.getEnvironment().brdfLUT,
                                 ImVec2(width, height));
    } else {
      ImGui::Button("Set BRDF", ImVec2(width, height));
    }

    if (ImGui::BeginDragDropTarget()) {
      if (auto *payload = ImGui::AcceptDragDropPayload(
              liquid::getAssetTypeString(liquid::AssetType::Texture).c_str())) {
        auto handle = *static_cast<liquid::TextureAssetHandle *>(payload->Data);
        auto &asset = assetManager.getRegistry().getTextures().getAsset(handle);

        editorManager.getEnvironment().brdfLUT = asset.data.deviceHandle;
      }
    }

    ImGui::EndTabItem();
  }
}

} // namespace liquidator
