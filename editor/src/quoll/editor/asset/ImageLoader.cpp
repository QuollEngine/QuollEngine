#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/renderer/TextureUtils.h"
#include "ImageLoader.h"
#include <stb_image.h>

namespace quoll::editor {

ImageLoader::ImageLoader(AssetCache &assetCache, RenderStorage &renderStorage)
    : mAssetCache(assetCache), mRenderStorage(renderStorage) {}

Result<Uuid> ImageLoader::loadFromPath(const Path &sourceAssetPath,
                                       const Uuid &uuid, bool generateMipMaps,
                                       rhi::Format format) {
  i32 width = 0;
  i32 height = 0;
  i32 channels = 0;

  auto *data = stbi_load(sourceAssetPath.string().c_str(), &width, &height,
                         &channels, STBI_rgb_alpha);

  if (!data) {
    return Result<Uuid>::Error(stbi_failure_reason());
  }

  auto res = loadFromMemory(
      data, static_cast<u32>(width), static_cast<u32>(height), uuid,
      sourceAssetPath.filename().string(), generateMipMaps, format);

  stbi_image_free(data);
  return res;
}

Result<Uuid> ImageLoader::loadFromMemory(void *data, u32 width, u32 height,
                                         const Uuid &uuid, const String &name,
                                         bool generateMipMaps,
                                         rhi::Format format) {
  std::vector<TextureAssetMipLevel> levels;
  std::vector<u8> assetData;
  if (generateMipMaps) {
    auto numLevels =
        static_cast<u32>(std::floor(std::log2(std::max(width, height)))) + 1;

    levels.resize(numLevels);

    u32 bufferOffset = 0;
    u32 mipWidth = width;
    u32 mipHeight = height;
    for (u32 i = 0; i < numLevels; ++i) {
      levels.at(i).offset = bufferOffset;
      levels.at(i).size = static_cast<usize>(mipWidth) * mipHeight * 4;
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
    levels.at(0).size = static_cast<usize>(width) * height * 4;
    levels.at(0).width = width;
    levels.at(0).height = height;
    usize textureSize = TextureUtils::getBufferSizeFromLevels(levels);
    assetData.resize(textureSize);
    memcpy(assetData.data(), data, textureSize);
  }

  AssetData<TextureAsset> asset{};
  asset.name = name;
  asset.uuid = uuid;
  asset.size = TextureUtils::getBufferSizeFromLevels(levels);
  asset.data.data = std::move(assetData);
  asset.data.height = height;
  asset.data.width = width;
  asset.data.layers = 1;
  asset.data.levels = levels;
  asset.data.format = format;

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);

  if (createdFileRes.hasError()) {
    return Result<Uuid>::Error(createdFileRes.getError());
  }

  auto loadRes = mAssetCache.loadTexture(asset.uuid);
  if (loadRes.hasError()) {
    return Result<Uuid>::Error(loadRes.getError());
  }

  return Result<Uuid>::Ok(
      mAssetCache.getRegistry().getTextures().getAsset(loadRes.getData()).uuid);
}

std::vector<u8> ImageLoader::generateMipMapsFromTextureData(
    void *data, const std::vector<TextureAssetMipLevel> &levels,
    rhi::Format format) {
  rhi::TextureDescription description{};
  description.mipLevelCount = static_cast<u32>(levels.size());
  description.width = levels.at(0).width;
  description.height = levels.at(0).height;
  description.depth = 1;
  description.usage = rhi::TextureUsage::TransferSource |
                      rhi::TextureUsage::TransferDestination |
                      rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  description.format = format;

  auto texture = mRenderStorage.createTexture(description);
  usize size = static_cast<usize>(description.width) * description.height * 4;

  TextureUtils::copyDataToTexture(
      mRenderStorage.getDevice(), data, texture,
      rhi::ImageLayout::TransferDestinationOptimal, 1,
      {TextureAssetMipLevel{0, size, description.width, description.height}});

  TextureUtils::generateMipMapsForTexture(
      mRenderStorage.getDevice(), texture,
      rhi::ImageLayout::TransferSourceOptimal, description.layerCount,
      static_cast<u32>(levels.size()), description.width, description.height);

  std::vector<u8> textureData(TextureUtils::getBufferSizeFromLevels(levels));

  TextureUtils::copyTextureToData(mRenderStorage.getDevice(), texture,
                                  rhi::ImageLayout::TransferDestinationOptimal,
                                  description.layerCount, levels,
                                  textureData.data());
  mRenderStorage.getDevice()->destroyTexture(texture);

  return std::move(textureData);
}

} // namespace quoll::editor
