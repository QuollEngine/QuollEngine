#include "core/Base.h"
#include <ktx.h>
#include <vulkan/vulkan.hpp>
#include <ktxvulkan.h>
#include "KtxTextureLoader.h"
#include "KtxError.h"

namespace liquid {

KtxTextureLoader::KtxTextureLoader(ResourceAllocator *resourceAllocator_)
    : resourceAllocator(resourceAllocator_) {}

SharedPtr<Texture> KtxTextureLoader::loadFromFile(const String &filename) {
  ktxTexture *ktxTextureData = nullptr;
  KTX_error_code result = ktxTexture_CreateFromNamedFile(
      filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
      &ktxTextureData);

  if (result != KTX_SUCCESS) {
    throw KtxError("Failed to load KTX file", result);
  }

  LIQUID_ASSERT(ktxTextureData->numDimensions >= 1 &&
                    ktxTextureData->numDimensions <= 3,
                "KTX texture dimensions must be 1 or 3");

  if (ktxTextureData->numDimensions == 1) {
    throw KtxError("1D textures are not supported", KTX_UNSUPPORTED_FEATURE);
  }

  if (ktxTextureData->numDimensions == 3) {
    throw KtxError("3D textures are not supported", KTX_UNSUPPORTED_FEATURE);
  }

  if (ktxTextureData->isArray) {
    throw KtxError("Texture arrays are not supported", KTX_UNSUPPORTED_FEATURE);
  }

  SharedPtr<Texture> texture = nullptr;

  if (ktxTextureData->isCubemap) {
    const size_t CUBE_NUM_FACES = 6;
    if (ktxTextureData->baseWidth != ktxTextureData->baseHeight) {
      throw KtxError("Cubemap width and height are not equal",
                     KTX_INVALID_VALUE);
    }

    size_t faceSize = ktxTexture_GetImageSize(ktxTextureData, 0);

    TextureCubemapData cubemapData;
    cubemapData.width = ktxTextureData->baseWidth;
    cubemapData.height = ktxTextureData->baseHeight;
    cubemapData.data = ktxTexture_GetData(ktxTextureData);
    cubemapData.size = faceSize * CUBE_NUM_FACES;

    // TODO: Abstract away the format type
    cubemapData.format = ktxTexture_GetVkFormat(ktxTextureData);

    for (size_t i = 0; i < cubemapData.faceData.size(); ++i) {
      size_t offset = 0;
      ktxTexture_GetImageOffset(ktxTextureData, 0, 0, i, &offset);

      cubemapData.faceData.at(i).offset = offset;
      cubemapData.faceData.at(i).size = faceSize;
    }

    texture = resourceAllocator->createTextureCubemap(cubemapData);
  } else {
    liquid::TextureData textureData;
    textureData.data = ktxTextureData->pData;
    textureData.width = ktxTextureData->baseWidth;
    textureData.height = ktxTextureData->baseHeight;

    texture = resourceAllocator->createTexture2D(textureData);
  }

  ktxTexture_Destroy(ktxTextureData);
  return texture;
}

} // namespace liquid
