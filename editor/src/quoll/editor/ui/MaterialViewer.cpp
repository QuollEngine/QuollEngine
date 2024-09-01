#include "quoll/core/Base.h"
#include "quoll/imgui/ImguiUtils.h"
#include "MaterialViewer.h"
#include "Widgets.h"

namespace quoll::editor {

static void renderTextureIfExists(widgets::Table &table, const String &label,
                                  const AssetRef<TextureAsset> &texture) {
  static constexpr glm::vec2 TextureSize(80.0f, 80.0f);

  if (texture) {
    table.column(label);
    table.column(texture, TextureSize);
  }
}

void MaterialViewer::render() {
  bool open = mMaterial;

  if (!open)
    return;

  if (!mMaterial) {
    mMaterial = AssetRef<MaterialAsset>();
    return;
  }

  const auto &material = mMaterial.get();

  String title = "Material: " + mMaterial.meta().name + "###MaterialViewer";

  if (auto _ = widgets::FixedWindow(title.c_str(), open)) {
    if (auto table = widgets::Table("TableRigidBodyDetails", 2)) {
      renderTextureIfExists(table, "Base texture", material.baseColorTexture);
      table.row("Base color factor", material.baseColorFactor);

      renderTextureIfExists(table, "Metallic roughness texture",
                            material.metallicRoughnessTexture);
      table.row("Metallic factor", material.metallicFactor);
      table.row("Roughness factor", material.roughnessFactor);

      renderTextureIfExists(table, "Normal texture", material.normalTexture);
      table.row("Normal scale", material.normalScale);

      renderTextureIfExists(table, "Occlusion texture",
                            material.occlusionTexture);
      table.row("Occlusion strength", material.occlusionStrength);

      renderTextureIfExists(table, "Emissive texture",
                            material.emissiveTexture);
      table.row("Emissive factor", material.emissiveFactor);
    }
  }

  if (!open) {
    mMaterial = AssetRef<MaterialAsset>();
  }
}

void MaterialViewer::open(AssetRef<MaterialAsset> material) {
  mMaterial = material;
}

} // namespace quoll::editor
