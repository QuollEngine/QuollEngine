#include "liquid/core/Base.h"
#include "AssetManager.h"

#include <ktx.h>
#include <vulkan/vulkan.h>

#include "liquid/loaders/KtxError.h"

namespace liquid {

AssetManager::AssetManager(const std::filesystem::path &assetsPath)
    : mAssetsPath(assetsPath) {}

std::filesystem::path
AssetManager::createTextureFromAsset(const AssetData<TextureAsset> &asset) {
  ktxTextureCreateInfo createInfo{};
  createInfo.baseWidth = asset.data.width;
  createInfo.baseHeight = asset.data.height;
  createInfo.baseDepth = 1;
  createInfo.numDimensions = 2;
  createInfo.numFaces = 1;
  createInfo.numLayers = 1;
  createInfo.numLevels = 1;
  createInfo.isArray = KTX_FALSE;
  createInfo.generateMipmaps = KTX_FALSE;
  createInfo.vkFormat = VK_FORMAT_R8G8B8A8_SRGB;

  std::filesystem::path assetPath = (mAssetsPath / (asset.name + ".ktx2"));

  ktxTexture2 *texture = nullptr;
  {
    auto res = ktxTexture2_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE,
                                  &texture);
    LIQUID_ASSERT(res == KTX_SUCCESS, "Cannot create KTX texture");
  }

  auto *baseTexture = reinterpret_cast<ktxTexture *>(texture);

  ktxTexture_SetImageFromMemory(
      baseTexture, 0, 0, 0, static_cast<const ktx_uint8_t *>(asset.data.data),
      asset.size);

  {
    auto res =
        ktxTexture_WriteToNamedFile(baseTexture, assetPath.string().c_str());
    LIQUID_ASSERT(res == KTX_SUCCESS, "Cannot write KTX texture to a file");
  }

  ktxTexture_Destroy(baseTexture);

  return assetPath;
}

TextureAssetHandle
AssetManager::loadTextureFromFile(const std::filesystem::path &filePath) {
  constexpr uint32_t CUBEMAP_SIDES = 6;

  ktxTexture *ktxTextureData = nullptr;
  KTX_error_code result = ktxTexture_CreateFromNamedFile(
      filePath.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
      &ktxTextureData);

  LIQUID_ASSERT(result == KTX_SUCCESS,
                KtxError("Failed to load KTX file", result).what());

  LIQUID_ASSERT(ktxTextureData->numDimensions == 2,
                "Only 2D textures are supported");

  LIQUID_ASSERT(
      !ktxTextureData->isArray,
      KtxError("Texture arrays are not supported", KTX_UNSUPPORTED_FEATURE)
          .what());

  AssetData<TextureAsset> texture{};
  texture.name = filePath.filename().string();
  texture.size = ktxTexture_GetDataSizeUncompressed(ktxTextureData);
  texture.data.data = new char[texture.size];
  texture.data.width = ktxTextureData->baseWidth;
  texture.data.height = ktxTextureData->baseHeight;
  texture.data.layers = ktxTextureData->numLayers *
                        (ktxTextureData->isCubemap ? CUBEMAP_SIDES : 1);

  char *srcData = reinterpret_cast<char *>(ktxTexture_GetData(ktxTextureData));

  if (ktxTextureData->isCubemap) {
    size_t faceSize = ktxTexture_GetImageSize(ktxTextureData, 0);

    char *dstData = static_cast<char *>(texture.data.data);

    for (size_t i = 0; i < CUBEMAP_SIDES; ++i) {
      size_t offset = 0;
      ktxTexture_GetImageOffset(ktxTextureData, 0, 0,
                                static_cast<ktx_uint32_t>(i), &offset);

      memcpy(dstData + faceSize * i, srcData + offset, faceSize);
    }
  } else {
    memcpy(texture.data.data, srcData, texture.size);
  }

  ktxTexture_Destroy(ktxTextureData);

  return mRegistry.getTextures().addAsset(texture);
}

} // namespace liquid
