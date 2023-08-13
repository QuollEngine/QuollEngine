#include "liquid/core/Base.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/renderer/TextureUtils.h"

#include "ImageLoader.h"

#include <stb/stb_image.h>

namespace liquid::editor {

ImageLoader::ImageLoader(AssetCache &assetCache, RenderStorage &renderStorage)
    : mAssetCache(assetCache), mRenderStorage(renderStorage) {}

Result<String> ImageLoader::loadFromPath(const Path &sourceAssetPath,
                                         const String &uuid,
                                         bool generateMipMaps,
                                         rhi::Format format) {
  int32_t width = 0;
  int32_t height = 0;
  int32_t channels = 0;

  auto *data = stbi_load(sourceAssetPath.string().c_str(), &width, &height,
                         &channels, STBI_rgb_alpha);

  if (!data) {
    return Result<String>::Error(stbi_failure_reason());
  }

  auto res = loadFromMemory(
      data, static_cast<uint32_t>(width), static_cast<uint32_t>(height), uuid,
      sourceAssetPath.filename().string(), generateMipMaps, format);

  stbi_image_free(data);
  return res;
}

Result<String> ImageLoader::loadFromMemory(void *data, uint32_t width,
                                           uint32_t height, const String &uuid,
                                           const String &name,
                                           bool generateMipMaps,
                                           rhi::Format format) {
  std::vector<TextureAssetLevel> levels;
  std::vector<uint8_t> assetData;
  if (generateMipMaps) {
    auto numLevels =
        static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) +
        1;

    levels.resize(numLevels);

    uint32_t bufferOffset = 0;
    uint32_t mipWidth = width;
    uint32_t mipHeight = height;
    for (uint32_t i = 0; i < numLevels; ++i) {
      levels.at(i).offset = bufferOffset;
      levels.at(i).size = static_cast<size_t>(mipWidth) * mipHeight * 4;
      levels.at(i).width = mipWidth;
      levels.at(i).height = mipHeight;

      bufferOffset += mipWidth * mipHeight * 4;

      if (mipWidth > 1) {
        mipWidth /= 2;
      }

      if (mipHeight > 1) {
        mipHeight /= 2;
      }
    }

    assetData = generateMipMapsFromTextureData(data, levels, format);
  } else {
    levels.resize(1);
    levels.at(0).offset = 0;
    levels.at(0).size = static_cast<size_t>(width) * height * 4;
    levels.at(0).width = width;
    levels.at(0).height = height;
    size_t textureSize = TextureUtils::getBufferSizeFromLevels(levels);
    assetData.resize(textureSize);
    memcpy(assetData.data(), data, textureSize);
  }

  AssetData<TextureAsset> asset{};
  asset.name = name;
  asset.size = TextureUtils::getBufferSizeFromLevels(levels);
  asset.data.data = std::move(assetData);
  asset.data.height = height;
  asset.data.width = width;
  asset.data.layers = 1;
  asset.data.levels = levels;
  asset.data.format = format;

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset, uuid);

  if (createdFileRes.hasError()) {
    return Result<String>::Error(createdFileRes.getError());
  }

  auto loadRes = mAssetCache.loadTextureFromFile(createdFileRes.getData());
  if (loadRes.hasError()) {
    return Result<String>::Error(loadRes.getError());
  }

  return Result<String>::Ok(
      mAssetCache.getRegistry().getTextures().getAsset(loadRes.getData()).uuid);
}

std::vector<uint8_t> ImageLoader::generateMipMapsFromTextureData(
    void *data, const std::vector<TextureAssetLevel> &levels,
    rhi::Format format) {
  rhi::TextureDescription description{};
  description.mipLevelCount = static_cast<uint32_t>(levels.size());
  description.width = levels.at(0).width;
  description.height = levels.at(0).height;
  description.depth = 1;
  description.usage = rhi::TextureUsage::TransferSource |
                      rhi::TextureUsage::TransferDestination |
                      rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  description.format = format;

  auto texture = mRenderStorage.createTexture(description);
  size_t size = static_cast<size_t>(description.width) * description.height * 4;

  TextureUtils::copyDataToTexture(
      mRenderStorage.getDevice(), data, texture,
      rhi::ImageLayout::TransferDestinationOptimal, 1,
      {TextureAssetLevel{0, size, description.width, description.height}});

  TextureUtils::generateMipMapsForTexture(
      mRenderStorage.getDevice(), texture,
      rhi::ImageLayout::TransferSourceOptimal, description.layerCount,
      static_cast<uint32_t>(levels.size()), description.width,
      description.height);

  std::vector<uint8_t> textureData(
      TextureUtils::getBufferSizeFromLevels(levels));

  TextureUtils::copyTextureToData(mRenderStorage.getDevice(), texture,
                                  rhi::ImageLayout::TransferDestinationOptimal,
                                  description.layerCount, levels,
                                  textureData.data());
  mRenderStorage.getDevice()->destroyTexture(texture);

  return std::move(textureData);
}

} // namespace liquid::editor
