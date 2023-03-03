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

      editorManager.setSkyboxTexture(handle);
    }
  }
}

static String getSkyboxTypeLabel(EditorManager &editorManager) {
  if (!editorManager.hasSkybox()) {
    return "None";
  }

  auto type = editorManager.getSkyboxType();
  if (type == EnvironmentSkyboxType::Color) {
    return "Color";
  }

  if (type == EnvironmentSkyboxType::Texture) {
    return "Texture";
  }

  return "None";
}

void EnvironmentPanel::render(EditorManager &editorManager,
                              AssetManager &assetManager) {

  auto &textures = assetManager.getAssetRegistry().getTextures();
  auto &environments = assetManager.getAssetRegistry().getEnvironments();

  if (auto _ = widgets::Window("Environment")) {
    if (auto section = widgets::Section("Skybox")) {
      float width = section.getClipRect().GetWidth();
      const float height = width * 0.5f;

      ImGui::Text("Type");
      if (ImGui::BeginCombo("###SkyboxType",
                            getSkyboxTypeLabel(editorManager).c_str())) {
        if (ImGui::Selectable("None")) {
          editorManager.removeSkybox();
        } else if (ImGui::Selectable("Color")) {
          editorManager.setSkyboxColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
        } else if (ImGui::Selectable("Texture")) {
          editorManager.setSkyboxTexture(EnvironmentAssetHandle::Invalid);
        }

        ImGui::EndCombo();
      }

      if (!editorManager.hasSkybox()) {
        // Do nothing
      } else if (editorManager.getSkyboxType() ==
                 EnvironmentSkyboxType::Color) {
        glm::vec4 color = editorManager.getSkyboxColor();
        if (widgets::InputColor("Color", color)) {
          editorManager.setSkyboxColor(color);
        }
      } else if (editorManager.getSkyboxType() ==
                 EnvironmentSkyboxType::Texture) {
        if (assetManager.getAssetRegistry().getEnvironments().hasAsset(
                editorManager.getSkyboxTexture())) {
          const auto &envAsset =
              environments.getAsset(editorManager.getSkyboxTexture());

          imgui::image(envAsset.preview, ImVec2(width, height), ImVec2(0, 0),
                       ImVec2(1, 1), ImGui::GetID("environment-texture-drop"));

          dndEnvironmentAsset(section, editorManager);

          if (ImGui::Button(fa::Times)) {
            editorManager.setSkyboxTexture(EnvironmentAssetHandle::Invalid);
          }

        } else {
          ImGui::Button("Drag environment asset here", ImVec2(width, height));
          dndEnvironmentAsset(section, editorManager);
        }
      }
    }

    if (auto _ = widgets::Section("Lighting")) {
      auto source = editorManager.getEnvironmentLightingSource();

      String sourceName = "None";
      if (source == EnvironmentLightingSource::Skybox) {
        sourceName = "Skybox";
      }

      ImGui::Text("Source");
      if (ImGui::BeginCombo("###LightingSource", sourceName.c_str())) {
        if (ImGui::Selectable("None")) {
          editorManager.removeEnvironmentLightingSource(true);
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
