#include "liquid/core/Base.h"

#include "TextureStep.h"

namespace liquid::editor {

void loadTextures(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &targetPath = importData.targetPath;
  const auto &model = importData.model;

  std::map<size_t, TextureAssetHandle> map;

  for (size_t i = 0; i < model.textures.size(); ++i) {
    // TODO: Support creating different samplers
    auto &image = model.images.at(model.textures.at(i).source);
    AssetData<TextureAsset> texture{};
    auto filename = "texture" + std::to_string(i);
    auto textureRelPath =
        std::filesystem::relative(targetPath, assetCache.getAssetsPath()) /
        filename;

    importData.imageLoader.loadFromMemory(
        const_cast<void *>(static_cast<const void *>(image.image.data())),
        image.width, image.height, textureRelPath, importData.optimize);

    auto handle =
        assetCache.getRegistry().getTextures().findHandleByRelativePath(
            textureRelPath.replace_extension("ktx2"));

    importData.textures.map.insert_or_assign(i, handle);
  }
}

} // namespace liquid::editor
