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
  const auto &targetPath = importData.targetPath;
  const auto &model = importData.model;

  auto &image = model.images.at(model.textures.at(index).source);

  rhi::Format format = rhi::Format::Undefined;

  if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
    format = colorSpace == GLTFTextureColorSpace::Srgb
                 ? rhi::Format::Rgba8Srgb
                 : rhi::Format::Rgba8Unorm;
  }

  if (format == rhi::Format::Undefined) {
    importData.warnings.push_back("Texture #" + std::to_string(index) +
                                  " has 16-bit channels and cannot be loaded");
    return TextureAssetHandle::Invalid;
  }

  AssetData<TextureAsset> texture{};
  auto assetName =
      image.name.empty() ? "texture" + std::to_string(index) : image.name;
  auto textureRelPath =
      std::filesystem::relative(targetPath, assetCache.getAssetsPath()) /
      assetName;

  importData.imageLoader.loadFromMemory(
      const_cast<void *>(static_cast<const void *>(image.image.data())),
      image.width, image.height, textureRelPath,
      generateMipMaps && importData.optimize, format);

  auto handle = assetCache.getRegistry().getTextures().findHandleByRelativePath(
      textureRelPath.replace_extension("ktx2"));

  importData.textures.map.insert_or_assign(index, handle);
  return handle;
}

} // namespace liquid::editor
