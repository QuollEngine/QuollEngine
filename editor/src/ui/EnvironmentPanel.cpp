#include "liquid/core/Base.h"
#include "EnvironmentPanel.h"

#include "liquid/imgui/ImguiUtils.h"

namespace liquidator {

void EnvironmentPanel::render(SceneManager &sceneManager,
                              liquid::AssetManager &assetManager) {
  static constexpr float HALF = 0.5f;
  if (ImGui::BeginTabItem("Environment", 0)) {
    float width = ImGui::GetWindowWidth();
    float height = width * HALF;

    if (liquid::rhi::isHandleValid(
            sceneManager.getEnvironment().irradianceMap)) {
      ImGui::Button("Set irradiance (texture set)", ImVec2(width, height));
    } else {
      ImGui::Button("Set irradiance", ImVec2(width, height));
    }

    if (ImGui::BeginDragDropTarget()) {
      if (auto *payload = ImGui::AcceptDragDropPayload(
              liquid::getAssetTypeString(liquid::AssetType::Texture).c_str())) {
        auto handle = *static_cast<liquid::TextureAssetHandle *>(payload->Data);
        auto &asset = assetManager.getRegistry().getTextures().getAsset(handle);

        sceneManager.getEnvironment().irradianceMap = asset.data.deviceHandle;
      }
    }

    if (liquid::rhi::isHandleValid(sceneManager.getEnvironment().specularMap)) {
      ImGui::Button("Set specular (texture set)", ImVec2(width, height));
    } else {
      ImGui::Button("Set specular", ImVec2(width, height));
    }

    if (ImGui::BeginDragDropTarget()) {
      if (auto *payload = ImGui::AcceptDragDropPayload(
              liquid::getAssetTypeString(liquid::AssetType::Texture).c_str())) {
        auto handle = *static_cast<liquid::TextureAssetHandle *>(payload->Data);
        auto &asset = assetManager.getRegistry().getTextures().getAsset(handle);

        sceneManager.getEnvironment().specularMap = asset.data.deviceHandle;
      }
    }

    if (liquid::rhi::isHandleValid(sceneManager.getEnvironment().brdfLUT)) {
      liquid::imgui::imageButton(sceneManager.getEnvironment().brdfLUT,
                                 ImVec2(width, height));
    } else {
      ImGui::Button("Set BRDF", ImVec2(width, height));
    }

    if (ImGui::BeginDragDropTarget()) {
      if (auto *payload = ImGui::AcceptDragDropPayload(
              liquid::getAssetTypeString(liquid::AssetType::Texture).c_str())) {
        auto handle = *static_cast<liquid::TextureAssetHandle *>(payload->Data);
        auto &asset = assetManager.getRegistry().getTextures().getAsset(handle);

        sceneManager.getEnvironment().brdfLUT = asset.data.deviceHandle;
      }
    }

    ImGui::EndTabItem();
  }
}

} // namespace liquidator
