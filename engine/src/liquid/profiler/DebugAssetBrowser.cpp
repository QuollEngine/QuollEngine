#include "liquid/core/Base.h"
#include "DebugAssetBrowser.h"

#include <imgui.h>

namespace liquid {

DebugAssetBrowser::DebugAssetBrowser(liquid::AssetRegistry &registry)
    : mRegistry(registry) {}

void DebugAssetBrowser::render() {
  const float ASSET_TYPE_WIDTH = 150.0f;
  const float ASSET_LIST_WIDTH = 200.0f;
  if (ImGui::Begin("Asset browser (Debug)", &mOpen,
                   ImGuiWindowFlags_NoDocking)) {
    if (ImGui::BeginChild("asset-type", ImVec2(ASSET_TYPE_WIDTH, 0))) {

      if (ImGui::Selectable("Textures", mSelectedType == AssetType::Texture)) {
        mSelectedType = AssetType::Texture;
        mSelectedObject = 0;
      }

      if (ImGui::Selectable("Materials",
                            mSelectedType == AssetType::Material)) {
        mSelectedType = AssetType::Material;
        mSelectedObject = 0;
      }

      if (ImGui::Selectable("Meshes", mSelectedType == AssetType::Mesh)) {
        mSelectedType = AssetType::Mesh;
        mSelectedObject = 0;
      }

      if (ImGui::Selectable("Skinned meshes",
                            mSelectedType == AssetType::SkinnedMesh)) {
        mSelectedType = AssetType::SkinnedMesh;
        mSelectedObject = 0;
      }

      ImGui::EndChild();
    }
    ImGui::SameLine();

    if (ImGui::BeginChild("asset-list", ImVec2(ASSET_LIST_WIDTH, 0))) {
      if (mSelectedType == AssetType::Texture) {
        for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
          uint32_t rawHandle = static_cast<uint32_t>(handle);
          if (ImGui::Selectable(asset.name.c_str(),
                                mSelectedObject == rawHandle)) {
            mSelectedObject = rawHandle;
          }
        }
      } else if (mSelectedType == AssetType::Material) {
        for (auto &[handle, asset] : mRegistry.getMaterials().getAssets()) {
          uint32_t rawHandle = static_cast<uint32_t>(handle);
          if (ImGui::Selectable(asset.name.c_str(),
                                mSelectedObject == rawHandle)) {
            mSelectedObject = rawHandle;
          }
        }
      } else if (mSelectedType == AssetType::Mesh) {
        for (auto &[handle, asset] : mRegistry.getMeshes().getAssets()) {
          uint32_t rawHandle = static_cast<uint32_t>(handle);
          if (ImGui::Selectable(asset.name.c_str(),
                                mSelectedObject == rawHandle)) {
            mSelectedObject = rawHandle;
          }
        }
      } else if (mSelectedType == AssetType::SkinnedMesh) {
        for (auto &[handle, asset] : mRegistry.getSkinnedMeshes().getAssets()) {
          uint32_t rawHandle = static_cast<uint32_t>(handle);
          if (ImGui::Selectable(asset.name.c_str(),
                                mSelectedObject == rawHandle)) {
            mSelectedObject = rawHandle;
          }
        }
      }

      ImGui::EndChild();
    }

    ImGui::SameLine();

    if (mSelectedObject > 0 && ImGui::BeginChild("asset-info")) {

      if (mOnLoadToScene && ImGui::Button("Load to scene")) {
        mOnLoadToScene(mSelectedType, mSelectedObject);
      }

      auto renderScalar = [](const liquid::String &label, float value) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(label.c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%0.2f", value);
      };

      auto renderScalar3 = [](const liquid::String &label,
                              const glm::vec3 &value) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(label.c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%0.2f, %0.2f, %0.2f", value.x, value.y, value.z);
      };

      auto renderScalar4 = [](const liquid::String &label,
                              const glm::vec4 &value) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(label.c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%0.2f, %0.2f, %0.2f, %0.2f", value.x, value.y, value.z,
                    value.w);
      };

      auto renderInt = [](const liquid::String &label, uint32_t value) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(label.c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%d", value);
      };

      auto renderString = [](const liquid::String &label,
                             const liquid::String &value) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text(label.c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%s", value.c_str());
      };

      auto renderTextureName = [=](const liquid::String &label,
                                   liquid::TextureAssetHandle value) {
        if (value != liquid::TextureAssetHandle::Invalid) {
          renderString(label, mRegistry.getTextures().getAsset(value).name);
        } else {
          renderString(label, "None");
        }
      };

      auto renderMaterialName = [=](const liquid::String &label,
                                    liquid::MaterialAssetHandle value) {
        if (value != liquid::MaterialAssetHandle::Invalid) {
          renderString(label, mRegistry.getMaterials().getAsset(value).name);
        } else {
          renderString(label, "None");
        }
      };

      if (mSelectedType == AssetType::Texture) {
        auto handle = static_cast<liquid::TextureAssetHandle>(mSelectedObject);
        auto &texture = mRegistry.getTextures().getAsset(handle);
        if (ImGui::BeginTable("texture-info", 2,
                              ImGuiTableFlags_Borders |
                                  ImGuiTableColumnFlags_WidthStretch |
                                  ImGuiTableFlags_RowBg)) {
          renderString("Name", texture.name);
          renderInt("Size", static_cast<uint32_t>(texture.size));
          renderInt("Width", texture.data.width);
          renderInt("Height", texture.data.width);
          renderInt("Device handle",
                    static_cast<uint32_t>(texture.data.deviceHandle));

          ImGui::EndTable();
        }
      } else if (mSelectedType == AssetType::Material) {
        auto handle = static_cast<liquid::MaterialAssetHandle>(mSelectedObject);
        auto &material = mRegistry.getMaterials().getAsset(handle);
        if (ImGui::BeginTable("texture-info", 2,
                              ImGuiTableFlags_Borders |
                                  ImGuiTableColumnFlags_WidthStretch |
                                  ImGuiTableFlags_RowBg)) {
          renderTextureName("Base color texture",
                            material.data.baseColorTexture);
          renderScalar4("Base color factor", material.data.baseColorFactor);

          renderTextureName("Metallic roughness map",
                            material.data.metallicRoughnessTexture);
          renderScalar("Metallic factor", material.data.metallicFactor);
          renderScalar("Roughness factor", material.data.roughnessFactor);

          renderTextureName("Normal map", material.data.normalTexture);
          renderScalar("Normal scale", material.data.normalScale);

          renderTextureName("Occlusion map", material.data.occlusionTexture);
          renderScalar("Occlusion strength", material.data.occlusionStrength);

          renderTextureName("Emissive map", material.data.emissiveTexture);
          renderScalar3("Emissive factor", material.data.emissiveFactor);

          ImGui::EndTable();
        }
      } else if (mSelectedType == AssetType::Mesh) {
        auto handle = static_cast<liquid::MeshAssetHandle>(mSelectedObject);
        auto &mesh = mRegistry.getMeshes().getAsset(handle);

        uint32_t geom = 1;
        for (auto &geometry : mesh.data.geometries) {
          ImGui::Text("Geometry #%d", geom++);
          if (ImGui::BeginTable("texture-info", 2,
                                ImGuiTableFlags_Borders |
                                    ImGuiTableColumnFlags_WidthStretch |
                                    ImGuiTableFlags_RowBg)) {
            renderInt("Number of vertices",
                      static_cast<uint32_t>(geometry.vertices.size()));
            renderInt("Number of indices",
                      static_cast<uint32_t>(geometry.indices.size()));
            renderMaterialName("Material", geometry.material);

            ImGui::EndTable();
          }
        }
      } else if (mSelectedType == AssetType::SkinnedMesh) {
        auto handle =
            static_cast<liquid::SkinnedMeshAssetHandle>(mSelectedObject);
        auto &mesh = mRegistry.getSkinnedMeshes().getAsset(handle);

        uint32_t geom = 1;
        for (auto &geometry : mesh.data.geometries) {
          ImGui::Text("Geometry #%d", geom++);
          if (ImGui::BeginTable("texture-info", 2,
                                ImGuiTableFlags_Borders |
                                    ImGuiTableColumnFlags_WidthStretch |
                                    ImGuiTableFlags_RowBg)) {
            renderInt("Number of vertices",
                      static_cast<uint32_t>(geometry.vertices.size()));
            renderInt("Number of indices",
                      static_cast<uint32_t>(geometry.indices.size()));
            renderMaterialName("Material", geometry.material);

            ImGui::EndTable();
          }
        }
      }

      ImGui::EndChild();
    }
  }
  ImGui::End();
}

void DebugAssetBrowser::setOnLoadToScene(
    std::function<void(AssetType, uint32_t)> &&handler) {
  mOnLoadToScene = handler;
}

} // namespace liquid
