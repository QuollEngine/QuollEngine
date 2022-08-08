#include "liquid/core/Base.h"
#include "MaterialViewer.h"

#include "liquid/imgui/ImguiUtils.h"
#include "Widgets.h"

namespace liquidator {

static void renderTextureIfExists(const liquid::String &label,
                                  liquid::TextureAssetHandle handle,
                                  liquid::AssetRegistry &assetRegistry) {
  static constexpr glm::vec2 TextureSize(80.0f, 80.0f);

  if (assetRegistry.getTextures().hasAsset(handle)) {
    auto texture =
        assetRegistry.getTextures().getAsset(handle).data.deviceHandle;

    widgets::Table::column(label);
    widgets::Table::column(texture, TextureSize);
  }
}

void MaterialViewer::render(liquid::AssetRegistry &assetRegistry) {
  bool open = mHandle != liquid::MaterialAssetHandle::Invalid;

  if (!open)
    return;

  if (!assetRegistry.getMaterials().hasAsset(mHandle)) {
    mHandle = liquid::MaterialAssetHandle::Invalid;
    return;
  }

  const auto &material = assetRegistry.getMaterials().getAsset(mHandle);

  liquid::String title = "Material: " + material.name + "###MaterialViewer";

  if (widgets::FixedWindow::begin(title.c_str(), open)) {
    if (widgets::Table::begin("TableRigidBodyDetails", 2)) {
      widgets::Table::row("Path", material.relativePath.string());

      renderTextureIfExists("Base texture", material.data.baseColorTexture,
                            assetRegistry);
      widgets::Table::row("Base color factor", material.data.baseColorFactor);

      renderTextureIfExists("Metallic roughness texture",
                            material.data.metallicRoughnessTexture,
                            assetRegistry);
      widgets::Table::row("Metallic factor", material.data.metallicFactor);
      widgets::Table::row("Roughness factor", material.data.roughnessFactor);

      renderTextureIfExists("Normal texture", material.data.normalTexture,
                            assetRegistry);
      widgets::Table::row("Normal scale", material.data.normalScale);

      renderTextureIfExists("Occlusion texture", material.data.occlusionTexture,
                            assetRegistry);
      widgets::Table::row("Occlusion strength",
                          material.data.occlusionStrength);

      renderTextureIfExists("Emissive texture", material.data.emissiveTexture,
                            assetRegistry);
      widgets::Table::row("Emissive factor", material.data.emissiveFactor);

      widgets::Table::end();
    }
  }

  widgets::FixedWindow::end();

  if (!open) {
    mHandle = liquid::MaterialAssetHandle::Invalid;
  }
}

void MaterialViewer::open(liquid::MaterialAssetHandle handle) {
  mHandle = handle;
}

} // namespace liquidator
