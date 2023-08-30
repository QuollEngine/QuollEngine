#include "liquid/core/Base.h"
#include "MaterialViewer.h"

#include "liquid/imgui/ImguiUtils.h"
#include "Widgets.h"

namespace quoll::editor {

static void renderTextureIfExists(widgets::Table &table, const String &label,
                                  TextureAssetHandle handle,
                                  AssetRegistry &assetRegistry) {
  static constexpr glm::vec2 TextureSize(80.0f, 80.0f);

  if (assetRegistry.getTextures().hasAsset(handle)) {
    auto texture =
        assetRegistry.getTextures().getAsset(handle).data.deviceHandle;

    table.column(label);
    table.column(texture, TextureSize);
  }
}

void MaterialViewer::render(AssetRegistry &assetRegistry) {
  bool open = mHandle != MaterialAssetHandle::Null;

  if (!open)
    return;

  if (!assetRegistry.getMaterials().hasAsset(mHandle)) {
    mHandle = MaterialAssetHandle::Null;
    return;
  }

  const auto &material = assetRegistry.getMaterials().getAsset(mHandle);

  String title = "Material: " + material.name + "###MaterialViewer";

  if (auto _ = widgets::FixedWindow(title.c_str(), open)) {
    if (auto table = widgets::Table("TableRigidBodyDetails", 2)) {
      renderTextureIfExists(table, "Base texture",
                            material.data.baseColorTexture, assetRegistry);
      table.row("Base color factor", material.data.baseColorFactor);

      renderTextureIfExists(table, "Metallic roughness texture",
                            material.data.metallicRoughnessTexture,
                            assetRegistry);
      table.row("Metallic factor", material.data.metallicFactor);
      table.row("Roughness factor", material.data.roughnessFactor);

      renderTextureIfExists(table, "Normal texture",
                            material.data.normalTexture, assetRegistry);
      table.row("Normal scale", material.data.normalScale);

      renderTextureIfExists(table, "Occlusion texture",
                            material.data.occlusionTexture, assetRegistry);
      table.row("Occlusion strength", material.data.occlusionStrength);

      renderTextureIfExists(table, "Emissive texture",
                            material.data.emissiveTexture, assetRegistry);
      table.row("Emissive factor", material.data.emissiveFactor);
    }
  }

  if (!open) {
    mHandle = MaterialAssetHandle::Null;
  }
}

void MaterialViewer::open(MaterialAssetHandle handle) { mHandle = handle; }

} // namespace quoll::editor
