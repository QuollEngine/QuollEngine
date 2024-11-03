#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

// Vulkan includes
#include <vulkan/vulkan.hpp>

// KTX includes
#include <ktx.h>
#include <ktxvulkan.h>

namespace quoll {

namespace {

// Reference:
// https://www.khronos.org/ktx/documentation/libktx/ktx_8h.html#a8de6927e772cc95a9f49593c3dd72069
static const std::map<ktx_error_code_e, String> KtxErrorCodeMap{
    // Success codes
    {KTX_SUCCESS, "Operation successfully completed"},
    {KTX_FILE_DATA_ERROR, "The data in the file is inconsistent with the spec"},
    {KTX_FILE_ISPIPE, "The file is a pipe or named pipe"},
    {KTX_FILE_OPEN_FAILED, "File could not be opened"},
    {KTX_FILE_OVERFLOW, "Operation exceeds maximum file size"},
    {KTX_FILE_READ_ERROR, "Failed to read from file"},
    {KTX_FILE_SEEK_ERROR, "Failed to seek from file"},
    {KTX_FILE_UNEXPECTED_EOF, "File is not complete"},
    {KTX_FILE_WRITE_ERROR, "Failed to write to file"},
    {KTX_INVALID_OPERATION, "Invalid operation"},
    {KTX_INVALID_VALUE, "Invalid parameter value"},
    {KTX_NOT_FOUND, "Requested key not found"},
    {KTX_OUT_OF_MEMORY, "Not enough memory to complete the operation"},
    {KTX_TRANSCODE_FAILED, "Transcoding block compressed texture failed"},
    {KTX_UNKNOWN_FILE_FORMAT, "File is not in KTX format"},
    {KTX_UNSUPPORTED_TEXTURE_TYPE, "Texture type not supported"},
    {KTX_UNSUPPORTED_FEATURE, "Feature not supported"},
};

Error createKtxError(const String &message, ktx_error_code_e resultCode) {
  const String errorMessage = "[KtxError] " + message;
  if (resultCode == KTX_SUCCESS) {
    return errorMessage;
  }

  const auto &codeString = "(code: " + std::to_string(resultCode) + ")";
  const auto &it = KtxErrorCodeMap.find(resultCode);
  const auto &humanReadableResultString =
      it != KtxErrorCodeMap.end() ? (*it).second : "Unknown Error";

  return errorMessage + ": " + humanReadableResultString + " " + codeString;
}

} // namespace

static constexpr u32 CubemapSides = 6;

constexpr VkFormat getVulkanFormatFromFormat(rhi::Format format) {
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

constexpr rhi::Format getFormatFromVulkanFormat(VkFormat format) {
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

Result<void> AssetCache::createTextureFromData(const TextureAsset &data,
                                               const Path &assetPath) {
  ktxTextureCreateInfo createInfo{};
  createInfo.baseWidth = data.width;
  createInfo.baseHeight = data.height;
  createInfo.baseDepth = 1;
  createInfo.numDimensions = 2;
  createInfo.numFaces =
      data.type == quoll::TextureAssetType::Cubemap ? CubemapSides : 1;
  createInfo.numLayers = data.layers;
  createInfo.numLevels = static_cast<u32>(data.levels.size());
  createInfo.isArray = KTX_FALSE;
  createInfo.generateMipmaps = KTX_FALSE;
  createInfo.vkFormat = getVulkanFormatFromFormat(data.format);

  ktxTexture2 *texture = nullptr;
  {
    auto res = ktxTexture2_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE,
                                  &texture);

    if (res != KTX_SUCCESS) {
      return createKtxError("Cannot create KTX texture", res);
    }
  }

  auto *baseTexture = reinterpret_cast<ktxTexture *>(texture);

  const auto *baseData = static_cast<const ktx_uint8_t *>(data.data.data());
  for (usize i = 0; i < data.levels.size(); ++i) {
    const auto &level = data.levels.at(i);

    usize faceOffset = 0;
    const usize faceSize = level.size / createInfo.numFaces;
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
      return createKtxError("Cannot write KTX texture to a file", res);
    }
  }

  ktxTexture_Destroy(baseTexture);

  return Ok();
}

Result<TextureAsset> AssetCache::loadTexture(const Path &path) {
  std::ifstream stream(path, std::ios::binary);
  if (!stream.good()) {
    return Error("Cannot open file: " + path.string());
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
    return createKtxError("Cannot load KTX texture", result);
  }

  if (ktxTextureData->numDimensions != 2) {
    return Error("Only 2D textures are supported");
  }

  if (ktxTextureData->isArray) {
    return Error("Texture arrays are not supported");
  }

  TextureAsset texture{};
  texture.size = ktxTexture_GetDataSizeUncompressed(ktxTextureData);
  texture.data.resize(texture.size);
  texture.width = ktxTextureData->baseWidth;
  texture.height = ktxTextureData->baseHeight;
  texture.layers = ktxTextureData->numLayers *
                   (ktxTextureData->isCubemap ? CubemapSides : 1);
  texture.type = ktxTextureData->isCubemap ? TextureAssetType::Cubemap
                                           : TextureAssetType::Standard;
  texture.format =
      getFormatFromVulkanFormat(ktxTexture_GetVkFormat(ktxTextureData));
  texture.levels.resize(ktxTextureData->numLevels);

  auto *srcData = ktxTexture_GetData(ktxTextureData);

  const usize numFaces = ktxTextureData->isCubemap ? CubemapSides : 1;

  usize levelOffset = 0;
  u32 mipWidth = texture.width;
  u32 mipHeight = texture.height;

  for (usize level = 0; level < texture.levels.size(); ++level) {
    // ktsTexture_GetImageSize returns size of a
    // single face or layer within a level
    const usize blockSize =
        ktxTexture_GetImageSize(ktxTextureData, static_cast<i32>(level));

    const usize levelSize = blockSize * numFaces;

    texture.levels.at(level).offset = static_cast<u32>(levelOffset);
    texture.levels.at(level).size = static_cast<u32>(levelSize);
    texture.levels.at(level).width = mipWidth;
    texture.levels.at(level).height = mipHeight;

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

      memcpy(static_cast<u8 *>(texture.data.data()) + levelOffset +
                 (blockSize * face),
             srcData + offset, blockSize);
    }
    levelOffset += levelSize;
  }

  ktxTexture_Destroy(ktxTextureData);

  return texture;
}

} // namespace quoll
