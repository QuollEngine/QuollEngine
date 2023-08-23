#include "liquid/core/Base.h"

#include "TextureUtils.h"

namespace liquid::editor {

TextureAssetHandle loadTexture(GLTFImportData &importData, size_t index,
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
    return TextureAssetHandle::Null;
  }

  auto prevUuid = getUUIDFromMap(importData.uuids, assetName);

  auto uuid = importData.imageLoader.loadFromMemory(
      const_cast<void *>(static_cast<const void *>(image.image.data())),
      image.width, image.height, prevUuid,
      getGLTFAssetName(importData, assetName),
      generateMipMaps && importData.optimize, format);

  if (uuid.hasError()) {
    importData.warnings.push_back(assetName + " could not be loaded");
    return TextureAssetHandle::Null;
  }

  auto handle =
      assetCache.getRegistry().getTextures().findHandleByUuid(uuid.getData());

  importData.outputUuids.insert_or_assign(
      assetName, assetCache.getRegistry().getTextures().getAsset(handle).uuid);

  importData.textures.map.insert_or_assign(index, handle);
  return handle;
}

} // namespace liquid::editor
