#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "AssetManager.h"
#include "AssetFileHeader.h"

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

std::filesystem::path
AssetManager::createMaterialFromAsset(const AssetData<MaterialAsset> &asset) {
  String extension = ".lqmat";

  std::filesystem::path assetPath = (mAssetsPath / (asset.name + extension));

  std::ofstream file(assetPath, std::ios::binary | std::ios::out);

  LIQUID_ASSERT(file.good(), "File cannot be created for writing");

  AssetFileHeader header{};
  header.type = AssetType::Material;
  header.version = createVersion(0, 1);

  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(reinterpret_cast<const char *>(&(header.version)),
             sizeof(header.version));
  file.write(reinterpret_cast<const char *>(&header.type), sizeof(header.type));

  auto getTextureRelativePath = [=](TextureAssetHandle handle) {
    if (handle != TextureAssetHandle::Invalid) {
      auto &texture = mRegistry.getTextures().getAsset(handle);
      auto path = std::filesystem::relative(texture.path, mAssetsPath).string();
      std::replace(path.begin(), path.end(), '\\', '/');
      return path;
    }

    return String("");
  };

  auto baseColorTexturePath =
      getTextureRelativePath(asset.data.baseColorTexture);
  uint32_t baseColorTexturePathLength =
      static_cast<uint32_t>(baseColorTexturePath.length());
  file.write(reinterpret_cast<const char *>(&baseColorTexturePathLength),
             sizeof(uint32_t));
  file.write(baseColorTexturePath.c_str(), baseColorTexturePathLength);
  file.write(reinterpret_cast<const char *>(&asset.data.baseColorTextureCoord),
             sizeof(int8_t));
  file.write(reinterpret_cast<const char *>(
                 glm::value_ptr(asset.data.baseColorFactor)),
             sizeof(float) * 4);

  auto metallicRoughnessTexturePath =
      getTextureRelativePath(asset.data.metallicRoughnessTexture);
  uint32_t metallicRoughnessTexturePathLength =
      static_cast<uint32_t>(metallicRoughnessTexturePath.length());
  file.write(
      reinterpret_cast<const char *>(&metallicRoughnessTexturePathLength),
      sizeof(uint32_t));
  file.write(metallicRoughnessTexturePath.c_str(),
             metallicRoughnessTexturePathLength);
  file.write(
      reinterpret_cast<const char *>(&asset.data.metallicRoughnessTextureCoord),
      sizeof(int8_t));
  file.write(reinterpret_cast<const char *>(&asset.data.metallicFactor),
             sizeof(float));
  file.write(reinterpret_cast<const char *>(&asset.data.roughnessFactor),
             sizeof(float));

  auto normalTexturePath = getTextureRelativePath(asset.data.normalTexture);
  uint32_t normalTexturePathLength =
      static_cast<uint32_t>(normalTexturePath.length());
  file.write(reinterpret_cast<const char *>(&normalTexturePathLength),
             sizeof(uint32_t));
  file.write(normalTexturePath.c_str(), normalTexturePathLength);
  file.write(reinterpret_cast<const char *>(&asset.data.normalTextureCoord),
             sizeof(int8_t));
  file.write(reinterpret_cast<const char *>(&asset.data.normalScale),
             sizeof(float));

  auto occlusionTexturePath =
      getTextureRelativePath(asset.data.occlusionTexture);
  uint32_t occlusionTexturePathLength =
      static_cast<uint32_t>(occlusionTexturePath.length());
  file.write(reinterpret_cast<const char *>(&occlusionTexturePathLength),
             sizeof(uint32_t));
  file.write(occlusionTexturePath.c_str(), occlusionTexturePathLength);
  file.write(reinterpret_cast<const char *>(&asset.data.occlusionTextureCoord),
             sizeof(int8_t));
  file.write(reinterpret_cast<const char *>(&asset.data.occlusionStrength),
             sizeof(float));

  auto emissiveTexturePath = getTextureRelativePath(asset.data.emissiveTexture);
  uint32_t emissiveTexturePathLength =
      static_cast<uint32_t>(emissiveTexturePath.length());
  file.write(reinterpret_cast<const char *>(&emissiveTexturePathLength),
             sizeof(uint32_t));
  file.write(emissiveTexturePath.c_str(), emissiveTexturePathLength);
  file.write(reinterpret_cast<const char *>(&asset.data.emissiveTextureCoord),
             sizeof(int8_t));
  file.write(
      reinterpret_cast<const char *>(glm::value_ptr(asset.data.emissiveFactor)),
      sizeof(float) * 3);

  file.close();

  return assetPath;
}

MaterialAssetHandle
AssetManager::loadMaterialFromFile(const std::filesystem::path &filePath) {
  std::ifstream file(filePath, std::ios::binary | std::ios::in);

  LIQUID_ASSERT(file.good(), "File cannot be opened for reading");

  AssetFileHeader header;

  String magic(ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);
  file.read(reinterpret_cast<char *>(&header.version), sizeof(header.version));
  file.read(reinterpret_cast<char *>(&header.type), sizeof(header.type));

  LIQUID_ASSERT(magic == header.magic, "Data is not a liquid file");
  LIQUID_ASSERT(header.type == AssetType::Material, "File is not a material");

  AssetData<MaterialAsset> material{};
  material.path = filePath;
  material.name = filePath.filename().string();
  material.type = header.type;

  // Base color
  {
    uint32_t pathLength = 0;
    file.read(reinterpret_cast<char *>(&pathLength), sizeof(uint32_t));
    liquid::String texturePathStr(pathLength, '$');
    file.read(texturePathStr.data(), pathLength);

    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.baseColorTexture = handle;
        break;
      }
    }

    file.read(reinterpret_cast<char *>(&material.data.baseColorTextureCoord),
              sizeof(int8_t));
    file.read(reinterpret_cast<char *>(&material.data.baseColorFactor),
              sizeof(float) * 4);
  }

  // Metallic roughness
  {
    uint32_t pathLength = 0;
    file.read(reinterpret_cast<char *>(&pathLength), sizeof(uint32_t));
    liquid::String texturePathStr(pathLength, '$');
    file.read(texturePathStr.data(), pathLength);

    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.metallicRoughnessTexture = handle;
        break;
      }
    }

    file.read(
        reinterpret_cast<char *>(&material.data.metallicRoughnessTextureCoord),
        sizeof(int8_t));
    file.read(reinterpret_cast<char *>(&material.data.metallicFactor),
              sizeof(float));
    file.read(reinterpret_cast<char *>(&material.data.roughnessFactor),
              sizeof(float));
  }

  // Normal
  {
    uint32_t pathLength = 0;
    file.read(reinterpret_cast<char *>(&pathLength), sizeof(uint32_t));
    liquid::String texturePathStr(pathLength, '$');
    file.read(texturePathStr.data(), pathLength);

    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.normalTexture = handle;
        break;
      }
    }

    file.read(reinterpret_cast<char *>(&material.data.normalTextureCoord),
              sizeof(int8_t));
    file.read(reinterpret_cast<char *>(&material.data.normalScale),
              sizeof(float));
  }

  // Occlusion
  {
    uint32_t pathLength = 0;
    file.read(reinterpret_cast<char *>(&pathLength), sizeof(uint32_t));
    liquid::String texturePathStr(pathLength, '$');
    file.read(texturePathStr.data(), pathLength);

    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.occlusionTexture = handle;
        break;
      }
    }

    file.read(reinterpret_cast<char *>(&material.data.occlusionTextureCoord),
              sizeof(int8_t));
    file.read(reinterpret_cast<char *>(&material.data.occlusionStrength),
              sizeof(float));
  }

  // Emissive
  {
    uint32_t pathLength = 0;
    file.read(reinterpret_cast<char *>(&pathLength), sizeof(uint32_t));
    liquid::String texturePathStr(pathLength, '$');
    file.read(texturePathStr.data(), pathLength);

    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.emissiveTexture = handle;
        break;
      }
    }

    file.read(reinterpret_cast<char *>(&material.data.emissiveTextureCoord),
              sizeof(int8_t));
    file.read(reinterpret_cast<char *>(&material.data.emissiveFactor),
              sizeof(float) * 3);
  }

  file.close();

  return mRegistry.getMaterials().addAsset(material);
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
  texture.path = filePath;
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
