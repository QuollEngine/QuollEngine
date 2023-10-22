#include "quoll/core/Base.h"
#include "quoll/core/Version.h"

#include "AssetCache.h"

#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

#include "quoll/loaders/KtxError.h"

#include <vulkan/vulkan.hpp>
#include <ktx.h>
#include <ktxvulkan.h>

namespace quoll {

static constexpr u32 CubemapSides = 6;

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
    QuollAssert(false, "Undefined format");
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
    QuollAssert(false, "Undefined format");
    return rhi::Format::Undefined;
  }
}

Result<Path> AssetCache::createTextureFromSource(const Path &sourcePath,
                                                 const Uuid &uuid) {
  if (uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  using co = std::filesystem::copy_options;

  auto assetPath = getPathFromUuid(uuid);

  if (!std::filesystem::copy_file(sourcePath, assetPath,
                                  co::overwrite_existing)) {
    return Result<Path>::Error("Cannot create texture from source: " +
                               sourcePath.stem().string());
  }

  auto metaRes = createAssetMeta(AssetType::Texture,
                                 sourcePath.filename().string(), assetPath);

  if (!metaRes.hasData()) {
    std::filesystem::remove(assetPath);
    return Result<Path>::Error("Cannot create texture from source: " +
                               sourcePath.stem().string());
  }

  return Result<Path>::Ok(assetPath);
}

Result<Path>
AssetCache::createTextureFromAsset(const AssetData<TextureAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  ktxTextureCreateInfo createInfo{};
  createInfo.baseWidth = asset.data.width;
  createInfo.baseHeight = asset.data.height;
  createInfo.baseDepth = 1;
  createInfo.numDimensions = 2;
  createInfo.numFaces =
      asset.data.type == quoll::TextureAssetType::Cubemap ? CubemapSides : 1;
  createInfo.numLayers = asset.data.layers;
  createInfo.numLevels = static_cast<u32>(asset.data.levels.size());
  createInfo.isArray = KTX_FALSE;
  createInfo.generateMipmaps = KTX_FALSE;
  createInfo.vkFormat = getVulkanFormatFromFormat(asset.data.format);

  auto assetPath = getPathFromUuid(asset.uuid);

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
  for (usize i = 0; i < asset.data.levels.size(); ++i) {
    const auto &level = asset.data.levels.at(i);

    usize faceOffset = 0;
    usize faceSize = level.size / createInfo.numFaces;
    for (u32 face = 0; face < createInfo.numFaces; ++face) {
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

  auto metaRes = createAssetMeta(AssetType::Texture, asset.name, assetPath);
  if (metaRes.hasError()) {
    std::filesystem::remove(assetPath);
    return metaRes;
  }

  ktxTexture_Destroy(baseTexture);

  return Result<Path>::Ok(assetPath);
}

Result<TextureAssetHandle> AssetCache::loadTexture(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);
  std::ifstream stream(filePath, std::ios::binary);
  if (!stream.good()) {
    return Result<TextureAssetHandle>::Error("Cannot open file: " +
                                             filePath.string());
  }

  stream.seekg(0, std::ios::end);
  auto size = stream.tellg();
  stream.seekg(0, std::ios::beg);

  std::vector<u8> bytes(size);
  stream.read(reinterpret_cast<char *>(bytes.data()), size);
  stream.close();

  ktxTexture *ktxTextureData = nullptr;
  KTX_error_code result = ktxTexture_CreateFromMemory(
      bytes.data(), bytes.size(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
      &ktxTextureData);

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

  auto meta = getAssetMeta(uuid);

  AssetData<TextureAsset> texture{};
  // TODO: Remove size
  texture.size = ktxTexture_GetDataSizeUncompressed(ktxTextureData);
  texture.name = meta.name;
  texture.path = filePath;
  texture.uuid = Uuid(filePath.stem().string());
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

  usize numFaces = ktxTextureData->isCubemap ? CubemapSides : 1;

  usize levelOffset = 0;
  u32 mipWidth = texture.data.width;
  u32 mipHeight = texture.data.height;

  for (usize level = 0; level < texture.data.levels.size(); ++level) {
    // ktsTexture_GetImageSize returns size of a
    // single face or layer within a level
    usize blockSize =
        ktxTexture_GetImageSize(ktxTextureData, static_cast<i32>(level));

    usize levelSize = blockSize * numFaces;

    texture.data.levels.at(level).offset = static_cast<u32>(levelOffset);
    texture.data.levels.at(level).size = static_cast<u32>(levelSize);
    texture.data.levels.at(level).width = mipWidth;
    texture.data.levels.at(level).height = mipHeight;

    if (mipWidth > 1) {
      mipWidth /= 2;
    }

    if (mipHeight > 1) {
      mipHeight /= 2;
    }

    for (usize face = 0; face < numFaces; ++face) {
      usize offset = 0;
      ktxTexture_GetImageOffset(ktxTextureData, static_cast<i32>(level), 0,
                                static_cast<u32>(face), &offset);

      memcpy(static_cast<u8 *>(texture.data.data.data()) + levelOffset +
                 (blockSize * face),
             srcData + offset, blockSize);
    }
    levelOffset += levelSize;
  }

  ktxTexture_Destroy(ktxTextureData);

  return Result<TextureAssetHandle>::Ok(
      mRegistry.getTextures().addAsset(texture));
}

Result<TextureAssetHandle> AssetCache::getOrLoadTexture(const Uuid &uuid) {
  if (uuid.isEmpty()) {
    return Result<TextureAssetHandle>::Ok(TextureAssetHandle::Null);
  }

  auto handle = mRegistry.getTextures().findHandleByUuid(uuid);
  if (handle != TextureAssetHandle::Null) {
    return Result<TextureAssetHandle>::Ok(handle);
  }

  return loadTexture(uuid);
}

} // namespace quoll
