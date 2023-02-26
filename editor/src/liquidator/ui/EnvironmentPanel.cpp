#include "liquid/core/Base.h"
#include "EnvironmentPanel.h"

#include "liquid/imgui/ImguiUtils.h"
#include "FontAwesome.h"
#include "Widgets.h"

namespace liquid::editor {

static void dndEnvironmentAsset(widgets::Section &section,
                                EditorManager &editorManager) {
  static constexpr float DropBorderWidth = 3.5f;
  auto &g = *ImGui::GetCurrentContext();

  ImVec2 dropMin(section.getClipRect().Min.x + DropBorderWidth,
                 g.LastItemData.Rect.Min.y + DropBorderWidth);
  ImVec2 dropMax(section.getClipRect().Max.x - DropBorderWidth,
                 g.LastItemData.Rect.Max.y - DropBorderWidth);
  if (ImGui::BeginDragDropTargetCustom(ImRect(dropMin, dropMax),
                                       g.LastItemData.ID)) {
    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Environment).c_str())) {
      auto handle = *static_cast<EnvironmentAssetHandle *>(payload->Data);

      editorManager.setEnvironmentSkybox(handle);
    }
  }
}

void EnvironmentPanel::render(EditorManager &editorManager,
                              AssetManager &assetManager) {

  auto &textures = assetManager.getAssetRegistry().getTextures();
  auto &environments = assetManager.getAssetRegistry().getEnvironments();

  if (auto _ = widgets::Window("Environment")) {
    if (auto section = widgets::Section("Skybox")) {
      float width = section.getClipRect().GetWidth();
      const float height = width * 0.5f;

      if (editorManager.hasEnvironmentSkybox()) {
        const auto &envAsset =
            environments.getAsset(editorManager.getEnvironmentSkybox());

        imgui::image(envAsset.preview, ImVec2(width, height), ImVec2(0, 0),
                     ImVec2(1, 1), ImGui::GetID("environment-texture-drop"));

        dndEnvironmentAsset(section, editorManager);

        if (ImGui::Button(fa::Times)) {
          editorManager.deleteEnvironmentSkybox();
        }

      } else {
        ImGui::Button("Drag environment asset here", ImVec2(width, height));
        dndEnvironmentAsset(section, editorManager);
      }
    }

    if (auto _ = widgets::Section("Lighting")) {
      auto source = editorManager.getEnvironmentLightingSource();

      String sourceName = "None";
      if (source == EnvironmentLightingSource::Skybox) {
        sourceName = "Skybox";
      }

      if (ImGui::BeginCombo("Source", sourceName.c_str())) {
        if (ImGui::Selectable("None")) {
          editorManager.removeEnvironmentLightingSource();
        }

        if (ImGui::Selectable("Use skybox")) {
          editorManager.setEnvironmentLightingSkyboxSource();
        }

        ImGui::EndCombo();
      }
    }
  }
}

} // namespace liquid::editor
