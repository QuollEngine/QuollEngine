#include "liquid/core/Base.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/renderer/TextureUtils.h"

#include "ImageLoader.h"

#include <stb/stb_image.h>

namespace liquid::editor {

ImageLoader::ImageLoader(AssetCache &assetCache, rhi::RenderDevice *device)
    : mAssetCache(assetCache), mDevice(device) {}

Result<Path> ImageLoader::loadFromPath(const Path &originalAssetPath,
                                       const Path &engineAssetPath,
                                       bool generateMipMaps,
                                       rhi::Format format) {
  int32_t width = 0;
  int32_t height = 0;
  int32_t channels = 0;

  auto *data = stbi_load(originalAssetPath.string().c_str(), &width, &height,
                         &channels, STBI_rgb_alpha);

  if (!data) {
    return Result<Path>::Error(stbi_failure_reason());
  }

  auto res = loadFromMemory(data, static_cast<uint32_t>(width),
                            static_cast<uint32_t>(height), engineAssetPath,
                            generateMipMaps, format);

  stbi_image_free(data);
  return res;
}

Result<Path> ImageLoader::loadFromMemory(void *data, uint32_t width,
                                         uint32_t height,
                                         const Path &engineAssetPath,
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
  asset.name = engineAssetPath.string();
  asset.size = TextureUtils::getBufferSizeFromLevels(levels);
  asset.data.data = assetData.data();
  asset.data.height = height;
  asset.data.width = width;
  asset.data.layers = 1;
  asset.data.levels = levels;
  asset.data.format = format;

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);

  if (createdFileRes.hasError()) {
    return createdFileRes;
  }

  auto loadRes = mAssetCache.loadAsset(createdFileRes.getData());
  if (loadRes.hasError()) {
    return Result<Path>::Error(loadRes.getError());
  }

  return createdFileRes;
}

std::vector<uint8_t> ImageLoader::generateMipMapsFromTextureData(
    void *data, const std::vector<TextureAssetLevel> &levels,
    rhi::Format format) {
  rhi::TextureDescription description{};
  description.levels = static_cast<uint32_t>(levels.size());
  description.width = levels.at(0).width;
  description.height = levels.at(0).height;
  description.depth = 1;
  description.usage = rhi::TextureUsage::TransferSource |
                      rhi::TextureUsage::TransferDestination |
                      rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  description.format = format;

  auto texture = mDevice->createTexture(description);
  size_t size = static_cast<size_t>(description.width) * description.height * 4;

  TextureUtils::copyDataToTexture(
      mDevice, data, texture, rhi::ImageLayout::TransferDestinationOptimal, 1,
      {TextureAssetLevel{0, size, description.width, description.height}});

  TextureUtils::generateMipMapsForTexture(
      mDevice, texture, rhi::ImageLayout::TransferSourceOptimal,
      description.layers, static_cast<uint32_t>(levels.size()),
      description.width, description.height);

  std::vector<uint8_t> textureData(
      TextureUtils::getBufferSizeFromLevels(levels));

  TextureUtils::copyTextureToData(
      mDevice, texture, rhi::ImageLayout::TransferDestinationOptimal,
      description.layers, levels, textureData.data());
  mDevice->destroyTexture(texture);

  return std::move(textureData);
}

} // namespace liquid::editor
