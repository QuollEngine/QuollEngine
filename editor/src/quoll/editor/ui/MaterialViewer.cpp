#include "quoll/core/Base.h"
#include "quoll/imgui/ImguiUtils.h"
#include "MaterialViewer.h"
#include "Widgets.h"

namespace quoll::editor {

static void renderTextureIfExists(widgets::Table &table, const String &label,
                                  AssetHandle<TextureAsset> handle,
                                  AssetRegistry &assetRegistry) {
  static constexpr glm::vec2 TextureSize(80.0f, 80.0f);

  if (assetRegistry.has(handle)) {
    auto texture = assetRegistry.get<TextureAsset>(handle).deviceHandle;

    table.column(label);
    table.column(texture, TextureSize);
  }
}

void MaterialViewer::render(AssetRegistry &assetRegistry) {
  bool open = mHandle;

  if (!open)
    return;

  if (!assetRegistry.has(mHandle)) {
    mHandle = AssetHandle<MaterialAsset>();
    return;
  }

  const auto &material = assetRegistry.get(mHandle);

  String title =
      "Material: " + assetRegistry.getMeta(mHandle).name + "###MaterialViewer";

  if (auto _ = widgets::FixedWindow(title.c_str(), open)) {
    if (auto table = widgets::Table("TableRigidBodyDetails", 2)) {
      renderTextureIfExists(table, "Base texture", material.baseColorTexture,
                            assetRegistry);
      table.row("Base color factor", material.baseColorFactor);

      renderTextureIfExists(table, "Metallic roughness texture",
                            material.metallicRoughnessTexture, assetRegistry);
      table.row("Metallic factor", material.metallicFactor);
      table.row("Roughness factor", material.roughnessFactor);

      renderTextureIfExists(table, "Normal texture", material.normalTexture,
                            assetRegistry);
      table.row("Normal scale", material.normalScale);

      renderTextureIfExists(table, "Occlusion texture",
                            material.occlusionTexture, assetRegistry);
      table.row("Occlusion strength", material.occlusionStrength);

      renderTextureIfExists(table, "Emissive texture", material.emissiveTexture,
                            assetRegistry);
      table.row("Emissive factor", material.emissiveFactor);
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
