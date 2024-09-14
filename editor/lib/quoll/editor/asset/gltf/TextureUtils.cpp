#include "quoll/core/Base.h"
#include "TextureUtils.h"

namespace quoll::editor {

AssetRef<TextureAsset> loadTexture(GLTFImportData &importData, usize index,
                                   GLTFTextureColorSpace colorSpace,
                                   bool generateMipMaps) {
  if (importData.textures.map.find(index) != importData.textures.map.end()) {
    return importData.textures.map.at(index);
  }

  auto &assetCache = importData.assetCache;
  const auto &model = importData.model;

  auto &image = model.images.at(model.textures.at(index).source);
  auto assetName =
      image.name.empty() ? "texture" + std::to_string(index) : image.name;
  assetName += ".tex";

  rhi::Format format = rhi::Format::Undefined;

  if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
    format = colorSpace == GLTFTextureColorSpace::Srgb
                 ? rhi::Format::Rgba8Srgb
                 : rhi::Format::Rgba8Unorm;
  }

  if (format == rhi::Format::Undefined) {
    importData.warnings.push_back(assetName +
                                  " has 16-bit channels and cannot be loaded");
    return AssetRef<TextureAsset>();
  }

  auto prevUuid = getOrCreateUuidFromMap(importData.uuids, assetName);

  auto uuid = importData.imageLoader.loadFromMemory(
      const_cast<void *>(static_cast<const void *>(image.image.data())),
      image.width, image.height, prevUuid,
      getGLTFAssetName(importData, assetName),
      generateMipMaps && importData.optimize, format);

  if (!uuid) {
    importData.warnings.push_back(assetName + " could not be loaded");
    return AssetRef<TextureAsset>();
  }

  auto texture = assetCache.request<TextureAsset>(uuid);

  auto res = assetCache.waitForIdle(uuid);
  if (!res) {
    importData.warnings.push_back(res.error());
  }

  importData.outputUuids.insert_or_assign(assetName, uuid);
  importData.textures.map.insert_or_assign(index, texture);

  return texture;
}

} // namespace quoll::editor
