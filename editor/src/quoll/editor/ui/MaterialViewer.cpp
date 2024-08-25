#include "quoll/core/Base.h"
#include "quoll/imgui/ImguiUtils.h"
#include "MaterialViewer.h"
#include "Widgets.h"

namespace quoll::editor {

static void renderTextureIfExists(widgets::Table &table, const String &label,
                                  AssetHandle<TextureAsset> handle,
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
  bool open = mHandle;

  if (!open)
    return;

  if (!assetRegistry.getMaterials().hasAsset(mHandle)) {
    mHandle = AssetHandle<MaterialAsset>();
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
    mHandle = AssetHandle<MaterialAsset>();
  }
}

void MaterialViewer::open(AssetHandle<MaterialAsset> handle) {
  mHandle = handle;
}

} // namespace quoll::editor
