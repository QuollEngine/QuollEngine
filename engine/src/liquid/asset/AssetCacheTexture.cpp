#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetCache.h"

#include "AssetFileHeader.h"
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

Result<Path>
AssetCache::createTextureFromAsset(const AssetData<TextureAsset> &asset) {
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

  Path assetPath = (mAssetsPath / (asset.name + ".ktx2")).make_preferred();

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

  auto *baseData = static_cast<const ktx_uint8_t *>(asset.data.data);
  for (size_t i = 0; i < asset.data.levels.size(); ++i) {
    const auto &level = asset.data.levels.at(i);

    ktxTexture_SetImageFromMemory(baseTexture, static_cast<ktx_uint32_t>(i), 0,
                                  0, baseData + level.offset, level.size);
  }

  {
    auto res =
        ktxTexture_WriteToNamedFile(baseTexture, assetPath.string().c_str());

    if (res != KTX_SUCCESS) {
      return Result<Path>::Error(
          KtxError("Cannot write KTX texture to a file", res).what());
    }
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

  AssetData<TextureAsset> texture{};
  texture.size = ktxTexture_GetDataSizeUncompressed(ktxTextureData);
  texture.path = filePath;
  texture.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  texture.name = texture.relativePath.string();
  texture.data.data = new char[texture.size];
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
    size_t size =
        ktxTexture_GetImageSize(ktxTextureData, static_cast<int32_t>(level));

    texture.data.levels.at(level).offset = static_cast<uint32_t>(levelOffset);
    texture.data.levels.at(level).size = static_cast<uint32_t>(size);
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

      memcpy(static_cast<uint8_t *>(texture.data.data) + levelOffset +
                 (size * face),
             srcData + offset, size);
    }
    levelOffset += size;
  }

  ktxTexture_Destroy(ktxTextureData);

  return Result<TextureAssetHandle>::Ok(
      mRegistry.getTextures().addAsset(texture));
}

Result<TextureAssetHandle>
AssetCache::getOrLoadTextureFromPath(StringView relativePath) {
  if (relativePath.empty()) {
    return Result<TextureAssetHandle>::Ok(TextureAssetHandle::Invalid);
  }

  Path fullPath = (mAssetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
    if (asset.path == fullPath) {
      return Result<TextureAssetHandle>::Ok(handle);
    }
  }

  return loadTextureFromFile(fullPath);
}

} // namespace liquid
