#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetCache.h"

#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

#include "liquid/loaders/KtxError.h"

#include <vulkan/vulkan.hpp>
#include <ktx.h>
#include <ktxvulkan.h>

namespace liquid {

static constexpr uint32_t CubemapSides = 6;

/**
 * @brief Get Vulkan format from RHI format
 *
 * @param format Vulkan format
 * @return RHI format
 */
static VkFormat getVulkanFormatFromFormat(rhi::Format format) {
  switch (format) {
  case rhi::Format::Rgba8Unorm:
    return VK_FORMAT_R8G8B8A8_UNORM;
  case rhi::Format::Rgba8Srgb:
    return VK_FORMAT_R8G8B8A8_SRGB;
  case rhi::Format::Bgra8Srgb:
    return VK_FORMAT_B8G8R8A8_SRGB;
  case rhi::Format::Rgba16Float:
    return VK_FORMAT_R16G16B16A16_SFLOAT;
  case rhi::Format::Rg32Float:
    return VK_FORMAT_R32G32_SFLOAT;
  case rhi::Format::Rgb32Float:
    return VK_FORMAT_R32G32B32_SFLOAT;
  case rhi::Format::Rgba32Float:
    return VK_FORMAT_R32G32B32A32_SFLOAT;
  case rhi::Format::Rgba32Uint:
    return VK_FORMAT_R32G32B32A32_UINT;
  case rhi::Format::Depth16Unorm:
    return VK_FORMAT_D16_UNORM;
  case rhi::Format::Depth32Float:
    return VK_FORMAT_D32_SFLOAT;
  case rhi::Format::Undefined:
  default:
    LIQUID_ASSERT(false, "Undefined format");
    return VK_FORMAT_UNDEFINED;
  }
}

/**
 * @brief Get RHI format from Vulkan format
 *
 * @param format Vulkan format
 * @return RHI format
 */
static rhi::Format getFormatFromVulkanFormat(VkFormat format) {
  switch (format) {
  case VK_FORMAT_R8G8B8A8_UNORM:
    return rhi::Format::Rgba8Unorm;
  case VK_FORMAT_R8G8B8A8_SRGB:
    return rhi::Format::Rgba8Srgb;
  case VK_FORMAT_B8G8R8A8_SRGB:
    return rhi::Format::Bgra8Srgb;
  case VK_FORMAT_R16G16B16A16_SFLOAT:
    return rhi::Format::Rgba16Float;
  case VK_FORMAT_R32G32_SFLOAT:
    return rhi::Format::Rg32Float;
  case VK_FORMAT_R32G32B32_SFLOAT:
    return rhi::Format::Rgb32Float;
  case VK_FORMAT_R32G32B32A32_SFLOAT:
    return rhi::Format::Rgba32Float;
  case VK_FORMAT_R32G32B32A32_UINT:
    return rhi::Format::Rgba32Uint;
  case VK_FORMAT_D16_UNORM:
    return rhi::Format::Depth16Unorm;
  case VK_FORMAT_D32_SFLOAT:
    return rhi::Format::Depth32Float;
  case VK_FORMAT_UNDEFINED:
  default:
    LIQUID_ASSERT(false, "Undefined format");
    return rhi::Format::Undefined;
  }
}

Result<Path> liquid::AssetCache::createTextureFromSource(const Path &sourcePath,
                                                         const String &uuid) {
  using co = std::filesystem::copy_options;

  auto assetPath = createAssetPath(uuid);

  if (!std::filesystem::copy_file(sourcePath, assetPath,
                                  co::overwrite_existing)) {
    return Result<Path>::Error("Cannot create texture from source: " +
                               sourcePath.stem().string());
  }

  auto metaRes = createMetaFile(AssetType::Texture,
                                sourcePath.filename().string(), assetPath);

  if (!metaRes.hasData()) {
    std::filesystem::remove(assetPath);
    return Result<Path>::Error("Cannot create texture from source: " +
                               sourcePath.stem().string());
  }

  return Result<Path>::Ok(assetPath);
}

Result<Path>
AssetCache::createTextureFromAsset(const AssetData<TextureAsset> &asset,
                                   const String &uuid) {
  ktxTextureCreateInfo createInfo{};
  createInfo.baseWidth = asset.data.width;
  createInfo.baseHeight = asset.data.height;
  createInfo.baseDepth = 1;
  createInfo.numDimensions = 2;
  createInfo.numFaces =
      asset.data.type == liquid::TextureAssetType::Cubemap ? CubemapSides : 1;
  createInfo.numLayers = asset.data.layers;
  createInfo.numLevels = static_cast<uint32_t>(asset.data.levels.size());
  createInfo.isArray = KTX_FALSE;
  createInfo.generateMipmaps = KTX_FALSE;
  createInfo.vkFormat = getVulkanFormatFromFormat(asset.data.format);

  auto assetPath = createAssetPath(uuid);

  ktxTexture2 *texture = nullptr;
  {
    auto res = ktxTexture2_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE,
                                  &texture);

    if (res != KTX_SUCCESS) {
      return Result<Path>::Error(
          KtxError("Cannot create KTX texture", res).what());
    }
  }

  auto *baseTexture = reinterpret_cast<ktxTexture *>(texture);

  const auto *baseData =
      static_cast<const ktx_uint8_t *>(asset.data.data.data());
  for (size_t i = 0; i < asset.data.levels.size(); ++i) {
    const auto &level = asset.data.levels.at(i);

    size_t faceOffset = 0;
    size_t faceSize = level.size / createInfo.numFaces;
    for (uint32_t face = 0; face < createInfo.numFaces; ++face) {
      ktxTexture_SetImageFromMemory(
          baseTexture, static_cast<ktx_uint32_t>(i), 0, face,
          baseData + level.offset + faceOffset, faceSize);
      faceOffset += faceSize;
    }
  }

  {
    auto res =
        ktxTexture_WriteToNamedFile(baseTexture, assetPath.string().c_str());

    if (res != KTX_SUCCESS) {
      return Result<Path>::Error(
          KtxError("Cannot write KTX texture to a file", res).what());
    }
  }

  auto metaRes = createMetaFile(AssetType::Texture, asset.name, assetPath);
  if (metaRes.hasError()) {
    std::filesystem::remove(assetPath);
    return metaRes;
  }

  ktxTexture_Destroy(baseTexture);

  return Result<Path>::Ok(assetPath);
}

Result<TextureAssetHandle>
AssetCache::loadTextureFromFile(const Path &filePath) {
  FILE *stream = fopen(filePath.string().c_str(), "rb");
  if (!stream) {
    return Result<TextureAssetHandle>::Error("Cannot open file: " +
                                             filePath.string());
  }

  ktxTexture *ktxTextureData = nullptr;
  KTX_error_code result = ktxTexture_CreateFromStdioStream(
      stream, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTextureData);

  fclose(stream);

  if (result != KTX_SUCCESS) {
    return Result<TextureAssetHandle>::Error(
        KtxError("Cannot load KTX texture", result).what());
  }

  if (ktxTextureData->numDimensions != 2) {
    return Result<TextureAssetHandle>::Error("Only 2D textures are supported");
  }

  if (ktxTextureData->isArray) {
    return Result<TextureAssetHandle>::Error(
        "Texture arrays are not supported");
  }

  auto meta = getMetaFromUuid(filePath.stem().string());

  AssetData<TextureAsset> texture{};
  // TODO: Remove size
  texture.size = ktxTexture_GetDataSizeUncompressed(ktxTextureData);
  texture.name = meta.name;
  texture.path = filePath;
  texture.uuid = filePath.stem().string();
  texture.type = AssetType::Texture;
  texture.data.data.resize(texture.size);
  texture.data.width = ktxTextureData->baseWidth;
  texture.data.height = ktxTextureData->baseHeight;
  texture.data.layers = ktxTextureData->numLayers *
                        (ktxTextureData->isCubemap ? CubemapSides : 1);
  texture.data.type = ktxTextureData->isCubemap ? TextureAssetType::Cubemap
                                                : TextureAssetType::Standard;
  texture.data.format =
      getFormatFromVulkanFormat(ktxTexture_GetVkFormat(ktxTextureData));
  texture.data.levels.resize(ktxTextureData->numLevels);

  auto *srcData = ktxTexture_GetData(ktxTextureData);

  size_t numFaces = ktxTextureData->isCubemap ? CubemapSides : 1;

  size_t levelOffset = 0;
  uint32_t mipWidth = texture.data.width;
  uint32_t mipHeight = texture.data.height;

  for (size_t level = 0; level < texture.data.levels.size(); ++level) {
    // ktsTexture_GetImageSize returns size of a
    // single face or layer within a level
    size_t blockSize =
        ktxTexture_GetImageSize(ktxTextureData, static_cast<int32_t>(level));

    size_t levelSize = blockSize * numFaces;

    texture.data.levels.at(level).offset = static_cast<uint32_t>(levelOffset);
    texture.data.levels.at(level).size = static_cast<uint32_t>(levelSize);
    texture.data.levels.at(level).width = mipWidth;
    texture.data.levels.at(level).height = mipHeight;

    if (mipWidth > 1) {
      mipWidth /= 2;
    }

    if (mipHeight > 1) {
      mipHeight /= 2;
    }

    for (size_t face = 0; face < numFaces; ++face) {
      size_t offset = 0;
      ktxTexture_GetImageOffset(ktxTextureData, static_cast<int32_t>(level), 0,
                                static_cast<uint32_t>(face), &offset);

      memcpy(static_cast<uint8_t *>(texture.data.data.data()) + levelOffset +
                 (blockSize * face),
             srcData + offset, blockSize);
    }
    levelOffset += levelSize;
  }

  ktxTexture_Destroy(ktxTextureData);

  return Result<TextureAssetHandle>::Ok(
      mRegistry.getTextures().addAsset(texture));
}

Result<TextureAssetHandle>
AssetCache::getOrLoadTextureFromUuid(const String &uuid) {
  if (uuid.empty()) {
    return Result<TextureAssetHandle>::Ok(TextureAssetHandle::Null);
  }

  auto handle = mRegistry.getTextures().findHandleByUuid(uuid);
  if (handle != TextureAssetHandle::Null) {
    return Result<TextureAssetHandle>::Ok(handle);
  }

  return loadTextureFromFile(getPathFromUuid(uuid));
}

} // namespace liquid
