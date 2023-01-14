#include "liquid/core/Base.h"
#include "TextureStep.h"

namespace liquidator {

void loadTextures(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &targetPath = importData.targetPath;
  const auto &model = importData.model;

  std::map<size_t, liquid::TextureAssetHandle> map;

  for (size_t i = 0; i < model.textures.size(); ++i) {
    // TODO: Support creating different samplers
    auto &image = model.images.at(model.textures.at(i).source);
    liquid::AssetData<liquid::TextureAsset> texture{};
    auto textureRelPath =
        std::filesystem::relative(targetPath, assetCache.getAssetsPath());

    auto filename = "texture" + std::to_string(i);
    texture.name = (textureRelPath / filename).string();

    texture.type = liquid::AssetType::Texture;
    texture.size = image.width * image.height * 4;
    texture.data.data =
        const_cast<void *>(static_cast<const void *>(image.image.data()));
    texture.data.width = image.width;
    texture.data.height = image.height;

    auto &&texturePath = assetCache.createTextureFromAsset(texture);
    auto handle = assetCache.loadTextureFromFile(texturePath.getData());
    importData.textures.map.insert_or_assign(i, handle.getData());
  }
}

} // namespace liquidator
