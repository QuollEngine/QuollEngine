#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "AssetManager.h"
#include "AssetFileHeader.h"

#include <ktx.h>
#include <vulkan/vulkan.h>

#include "liquid/loaders/KtxError.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

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

std::filesystem::path
AssetManager::createMaterialFromAsset(const AssetData<MaterialAsset> &asset) {
  String extension = ".lqmat";

  std::filesystem::path assetPath = (mAssetsPath / (asset.name + extension));

  OutputBinaryStream file(assetPath);

  LIQUID_ASSERT(file.good(), "File cannot be created for writing");

  AssetFileHeader header{};
  header.type = AssetType::Material;
  header.version = createVersion(0, 1);

  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

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
  file.write(baseColorTexturePath);
  file.write(asset.data.baseColorTextureCoord);
  file.write(asset.data.baseColorFactor);

  auto metallicRoughnessTexturePath =
      getTextureRelativePath(asset.data.metallicRoughnessTexture);
  file.write(metallicRoughnessTexturePath);
  file.write(asset.data.metallicRoughnessTextureCoord);
  file.write(asset.data.metallicFactor);
  file.write(asset.data.roughnessFactor);

  auto normalTexturePath = getTextureRelativePath(asset.data.normalTexture);
  file.write(normalTexturePath);
  file.write(asset.data.normalTextureCoord);
  file.write(asset.data.normalScale);

  auto occlusionTexturePath =
      getTextureRelativePath(asset.data.occlusionTexture);
  file.write(occlusionTexturePath);
  file.write(asset.data.occlusionTextureCoord);
  file.write(asset.data.occlusionStrength);

  auto emissiveTexturePath = getTextureRelativePath(asset.data.emissiveTexture);
  file.write(emissiveTexturePath);
  file.write(asset.data.emissiveTextureCoord);
  file.write(asset.data.emissiveFactor);

  return assetPath;
}

MaterialAssetHandle
AssetManager::loadMaterialFromFile(const std::filesystem::path &filePath) {
  InputBinaryStream file(filePath);

  LIQUID_ASSERT(file.good(), "File cannot be opened for reading");

  AssetFileHeader header;

  String magic(ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);
  file.read(header.version);
  file.read(header.type);

  LIQUID_ASSERT(magic == header.magic, "Data is not a liquid file");
  LIQUID_ASSERT(header.type == AssetType::Material, "File is not a material");

  AssetData<MaterialAsset> material{};
  material.path = filePath;
  material.name = filePath.filename().string();
  material.type = header.type;

  // Base color
  {

    String texturePathStr;
    file.read(texturePathStr);
    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.baseColorTexture = handle;
        break;
      }
    }

    file.read(material.data.baseColorTextureCoord);
    file.read(material.data.baseColorFactor);
  }

  // Metallic roughness
  {
    String texturePathStr;
    file.read(texturePathStr);
    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.metallicRoughnessTexture = handle;
        break;
      }
    }

    file.read(material.data.metallicRoughnessTextureCoord);
    file.read(material.data.metallicFactor);
    file.read(material.data.roughnessFactor);
  }

  // Normal
  {
    String texturePathStr;
    file.read(texturePathStr);

    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.normalTexture = handle;
        break;
      }
    }

    file.read(material.data.normalTextureCoord);
    file.read(material.data.normalScale);
  }

  // Occlusion
  {
    String texturePathStr;
    file.read(texturePathStr);
    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.occlusionTexture = handle;
        break;
      }
    }

    file.read(material.data.occlusionTextureCoord);
    file.read(material.data.occlusionStrength);
  }

  // Emissive
  {
    String texturePathStr;
    file.read(texturePathStr);

    std::filesystem::path texturePath =
        (mAssetsPath / texturePathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
      if (asset.path == texturePath) {
        material.data.emissiveTexture = handle;
        break;
      }
    }

    file.read(material.data.emissiveTextureCoord);
    file.read(material.data.emissiveFactor);
  }

  return mRegistry.getMaterials().addAsset(material);
}

std::filesystem::path
AssetManager::createMeshFromAsset(const AssetData<MeshAsset> &asset) {
  String extension = ".lqmesh";
  std::filesystem::path assetPath = (mAssetsPath / (asset.name + extension));
  OutputBinaryStream file(assetPath);

  LIQUID_ASSERT(file.good(), "File cannot be created for writing");

  AssetFileHeader header{};
  header.type = AssetType::Mesh;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  auto numGeometries = static_cast<uint32_t>(asset.data.geometries.size());
  file.write(numGeometries);

  auto getMaterialRelativePath = [=](MaterialAssetHandle handle) {
    if (handle != MaterialAssetHandle::Invalid) {
      auto &texture = mRegistry.getMaterials().getAsset(handle);
      auto path = std::filesystem::relative(texture.path, mAssetsPath).string();
      std::replace(path.begin(), path.end(), '\\', '/');
      return path;
    }

    return String("");
  };

  for (auto &geometry : asset.data.geometries) {
    auto numVertices = static_cast<uint32_t>(geometry.vertices.size());
    file.write(numVertices);
    std::vector<glm::vec3> positions(numVertices);
    std::vector<glm::vec3> normals(numVertices);
    std::vector<glm::vec4> tangents(numVertices);
    std::vector<glm::vec2> texCoords0(numVertices);
    std::vector<glm::vec2> texCoords1(numVertices);

    for (uint32_t i = 0; i < numVertices; ++i) {
      const auto &vertex = geometry.vertices.at(i);
      positions.at(i) = glm::vec3(vertex.x, vertex.y, vertex.z);
      normals.at(i) = glm::vec3(vertex.nx, vertex.ny, vertex.nz);
      tangents.at(i) = glm::vec4(vertex.tx, vertex.ty, vertex.tz, vertex.tw);
      texCoords0.at(i) = glm::vec2(vertex.u0, vertex.v0);
      texCoords1.at(i) = glm::vec2(vertex.u1, vertex.v1);
    }

    file.write(positions);
    file.write(normals);
    file.write(tangents);
    file.write(texCoords0);
    file.write(texCoords1);

    auto numIndices = static_cast<uint32_t>(geometry.indices.size());
    file.write(numIndices);
    file.write(geometry.indices);

    auto materialPath = getMaterialRelativePath(geometry.material);
    file.write(materialPath);
  }

  return assetPath;
}

MeshAssetHandle
AssetManager::loadMeshFromFile(const std::filesystem::path &filePath) {
  InputBinaryStream file(filePath);

  AssetFileHeader header;

  String magic(ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);
  file.read(header.version);
  file.read(header.type);

  LIQUID_ASSERT(magic == header.magic, "Data is not a liquid file");
  LIQUID_ASSERT(header.type == AssetType::Mesh, "File is not a mesh");

  AssetData<MeshAsset> mesh{};
  mesh.path = filePath;
  mesh.name = filePath.filename().string();
  mesh.type = header.type;

  uint32_t numGeometries = 0;
  file.read(numGeometries);

  mesh.data.geometries.resize(numGeometries);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    file.read(numVertices);
    mesh.data.geometries.at(i).vertices.resize(numVertices);

    std::vector<glm::vec3> positions(numVertices);
    std::vector<glm::vec3> normals(numVertices);
    std::vector<glm::vec4> tangents(numVertices);
    std::vector<glm::vec2> texCoords0(numVertices);
    std::vector<glm::vec2> texCoords1(numVertices);

    file.read(positions);
    file.read(normals);
    file.read(tangents);
    file.read(texCoords0);
    file.read(texCoords1);

    for (uint32_t v = 0; v < numVertices; ++v) {
      auto &vertex = mesh.data.geometries.at(i).vertices.at(v);
      vertex.x = positions.at(v).x;
      vertex.y = positions.at(v).y;
      vertex.z = positions.at(v).z;

      vertex.nx = normals.at(v).x;
      vertex.ny = normals.at(v).y;
      vertex.nz = normals.at(v).z;

      vertex.tx = tangents.at(v).x;
      vertex.ty = tangents.at(v).y;
      vertex.tz = tangents.at(v).z;
      vertex.tw = tangents.at(v).w;

      vertex.u0 = texCoords0.at(v).x;
      vertex.v0 = texCoords0.at(v).y;

      vertex.u1 = texCoords1.at(v).x;
      vertex.v1 = texCoords1.at(v).y;
    }

    uint32_t numIndices = 0;
    file.read(numIndices);

    mesh.data.geometries.at(i).indices.resize(numIndices);
    file.read(mesh.data.geometries.at(i).indices);

    String materialPathStr;
    file.read(materialPathStr);

    std::filesystem::path materialPath =
        (mAssetsPath / materialPathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getMaterials().getAssets()) {
      if (asset.path == materialPath) {
        mesh.data.geometries.at(i).material = handle;
        break;
      }
    }
  }

  return mRegistry.getMeshes().addAsset(mesh);
}

std::filesystem::path AssetManager::createSkinnedMeshFromAsset(
    const AssetData<SkinnedMeshAsset> &asset) {

  String extension = ".lqmesh";
  std::filesystem::path assetPath = (mAssetsPath / (asset.name + extension));
  OutputBinaryStream file(assetPath);

  LIQUID_ASSERT(file.good(), "File cannot be created for writing");

  AssetFileHeader header{};
  header.type = AssetType::SkinnedMesh;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  auto numGeometries = static_cast<uint32_t>(asset.data.geometries.size());
  file.write(numGeometries);

  auto getMaterialRelativePath = [=](MaterialAssetHandle handle) {
    if (handle != MaterialAssetHandle::Invalid) {
      auto &texture = mRegistry.getMaterials().getAsset(handle);
      auto path = std::filesystem::relative(texture.path, mAssetsPath).string();
      std::replace(path.begin(), path.end(), '\\', '/');
      return path;
    }

    return String("");
  };

  for (auto &geometry : asset.data.geometries) {
    auto numVertices = static_cast<uint32_t>(geometry.vertices.size());
    file.write(numVertices);
    std::vector<glm::vec3> positions(numVertices);
    std::vector<glm::vec3> normals(numVertices);
    std::vector<glm::vec4> tangents(numVertices);
    std::vector<glm::vec2> texCoords0(numVertices);
    std::vector<glm::vec2> texCoords1(numVertices);
    std::vector<glm::uvec4> joints(numVertices);
    std::vector<glm::vec4> weights(numVertices);

    for (uint32_t i = 0; i < numVertices; ++i) {
      const auto &vertex = geometry.vertices.at(i);
      positions.at(i) = glm::vec3(vertex.x, vertex.y, vertex.z);
      normals.at(i) = glm::vec3(vertex.nx, vertex.ny, vertex.nz);
      tangents.at(i) = glm::vec4(vertex.tx, vertex.ty, vertex.tz, vertex.tw);
      texCoords0.at(i) = glm::vec2(vertex.u0, vertex.v0);
      texCoords1.at(i) = glm::vec2(vertex.u1, vertex.v1);
      joints.at(i) = glm::uvec4(vertex.j0, vertex.j1, vertex.j2, vertex.j3);
      weights.at(i) = glm::vec4(vertex.w0, vertex.w1, vertex.w2, vertex.w3);
    }

    file.write(positions);
    file.write(normals);
    file.write(tangents);
    file.write(texCoords0);
    file.write(texCoords1);
    file.write(joints);
    file.write(weights);

    auto numIndices = static_cast<uint32_t>(geometry.indices.size());
    file.write(numIndices);
    file.write(geometry.indices);

    auto materialPath = getMaterialRelativePath(geometry.material);
    file.write(materialPath);
  }

  return assetPath;
}

SkinnedMeshAssetHandle
AssetManager::loadSkinnedMeshFromFile(const std::filesystem::path &filePath) {
  InputBinaryStream file(filePath);

  AssetFileHeader header;

  String magic(ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);
  file.read(header.version);
  file.read(header.type);

  LIQUID_ASSERT(magic == header.magic, "Data is not a liquid file");
  LIQUID_ASSERT(header.type == AssetType::SkinnedMesh,
                "File is not a skinned mesh");

  AssetData<SkinnedMeshAsset> mesh{};
  mesh.path = filePath;
  mesh.name = filePath.filename().string();
  mesh.type = header.type;

  uint32_t numGeometries = 0;
  file.read(numGeometries);

  mesh.data.geometries.resize(numGeometries);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    file.read(numVertices);
    mesh.data.geometries.at(i).vertices.resize(numVertices);

    std::vector<glm::vec3> positions(numVertices);
    std::vector<glm::vec3> normals(numVertices);
    std::vector<glm::vec4> tangents(numVertices);
    std::vector<glm::vec2> texCoords0(numVertices);
    std::vector<glm::vec2> texCoords1(numVertices);
    std::vector<glm::uvec4> joints(numVertices);
    std::vector<glm::vec4> weights(numVertices);

    file.read(positions);
    file.read(normals);
    file.read(tangents);
    file.read(texCoords0);
    file.read(texCoords1);
    file.read(joints);
    file.read(weights);

    for (uint32_t v = 0; v < numVertices; ++v) {
      auto &vertex = mesh.data.geometries.at(i).vertices.at(v);
      vertex.x = positions.at(v).x;
      vertex.y = positions.at(v).y;
      vertex.z = positions.at(v).z;

      vertex.nx = normals.at(v).x;
      vertex.ny = normals.at(v).y;
      vertex.nz = normals.at(v).z;

      vertex.tx = tangents.at(v).x;
      vertex.ty = tangents.at(v).y;
      vertex.tz = tangents.at(v).z;
      vertex.tw = tangents.at(v).w;

      vertex.u0 = texCoords0.at(v).x;
      vertex.v0 = texCoords0.at(v).y;

      vertex.u1 = texCoords1.at(v).x;
      vertex.v1 = texCoords1.at(v).y;

      vertex.j0 = joints.at(v).x;
      vertex.j1 = joints.at(v).y;
      vertex.j2 = joints.at(v).z;
      vertex.j3 = joints.at(v).w;

      vertex.w0 = weights.at(v).x;
      vertex.w1 = weights.at(v).y;
      vertex.w2 = weights.at(v).z;
      vertex.w3 = weights.at(v).w;
    }

    uint32_t numIndices = 0;
    file.read(numIndices);

    mesh.data.geometries.at(i).indices.resize(numIndices);
    file.read(mesh.data.geometries.at(i).indices);

    String materialPathStr;
    file.read(materialPathStr);

    std::filesystem::path materialPath =
        (mAssetsPath / materialPathStr).make_preferred();

    for (auto &[handle, asset] : mRegistry.getMaterials().getAssets()) {
      if (asset.path == materialPath) {
        mesh.data.geometries.at(i).material = handle;
        break;
      }
    }
  }

  return mRegistry.getSkinnedMeshes().addAsset(mesh);
}

std::filesystem::path
AssetManager::createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset) {
  String extension = ".lqskel";
  std::filesystem::path assetPath = (mAssetsPath / (asset.name + extension));
  OutputBinaryStream file(assetPath);

  LIQUID_ASSERT(file.good(), "File cannot be created for writing");

  AssetFileHeader header{};
  header.type = AssetType::Skeleton;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  auto numJoints = static_cast<uint32_t>(asset.data.jointLocalPositions.size());
  file.write(numJoints);

  file.write(asset.data.jointLocalPositions);
  file.write(asset.data.jointLocalRotations);
  file.write(asset.data.jointLocalScales);
  file.write(asset.data.jointParents);
  file.write(asset.data.jointInverseBindMatrices);
  file.write(asset.data.jointNames);

  return assetPath;
}

SkeletonAssetHandle
AssetManager::loadSkeletonFromFile(const std::filesystem::path &filePath) {
  InputBinaryStream file(filePath);

  AssetFileHeader header;
  String magic(ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);
  file.read(header.version);
  file.read(header.type);

  LIQUID_ASSERT(magic == header.magic, "Data is not a liquid file");
  LIQUID_ASSERT(header.type == AssetType::Skeleton, "File is not a skeleton");

  AssetData<SkeletonAsset> skeleton{};
  skeleton.path = filePath;
  skeleton.name = filePath.filename().string();
  skeleton.type = header.type;

  uint32_t numJoints = 0;
  file.read(numJoints);

  skeleton.data.jointLocalPositions.resize(numJoints);
  skeleton.data.jointLocalRotations.resize(numJoints);
  skeleton.data.jointLocalScales.resize(numJoints);
  skeleton.data.jointParents.resize(numJoints);
  skeleton.data.jointInverseBindMatrices.resize(numJoints);
  skeleton.data.jointNames.resize(numJoints);

  file.read(skeleton.data.jointLocalPositions);
  file.read(skeleton.data.jointLocalRotations);
  file.read(skeleton.data.jointLocalScales);
  file.read(skeleton.data.jointParents);
  file.read(skeleton.data.jointInverseBindMatrices);
  file.read(skeleton.data.jointNames);

  return mRegistry.getSkeletons().addAsset(skeleton);
}

std::filesystem::path
AssetManager::createAnimationFromAsset(const AssetData<AnimationAsset> &asset) {
  String extension = ".lqanim";
  std::filesystem::path assetPath = (mAssetsPath / (asset.name + extension));
  OutputBinaryStream file(assetPath);

  LIQUID_ASSERT(file.good(), "File cannot be created for writing");

  AssetFileHeader header{};
  header.type = AssetType::Animation;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  file.write(asset.data.time);
  uint32_t numKeyframes = static_cast<uint32_t>(asset.data.keyframes.size());
  file.write(numKeyframes);

  for (auto &keyframe : asset.data.keyframes) {
    file.write(keyframe.target);
    file.write(keyframe.interpolation);
    file.write(keyframe.jointTarget);
    file.write(keyframe.joint);

    uint32_t numValues = static_cast<uint32_t>(keyframe.keyframeTimes.size());
    file.write(numValues);
    file.write(keyframe.keyframeTimes);
    file.write(keyframe.keyframeValues);
  }

  return assetPath;
}

AnimationAssetHandle
AssetManager::loadAnimationFromFile(const std::filesystem::path &filePath) {
  InputBinaryStream file(filePath);

  AssetFileHeader header;
  String magic(ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);
  file.read(header.version);
  file.read(header.type);

  LIQUID_ASSERT(magic == header.magic, "Data is not a liquid file");
  LIQUID_ASSERT(header.type == AssetType::Animation,
                "File is not an animation");

  AssetData<AnimationAsset> animation{};
  animation.path = filePath;
  animation.name = filePath.filename().string();
  animation.type = header.type;

  file.read(animation.data.time);
  uint32_t numKeyframes = 0;
  file.read(numKeyframes);
  animation.data.keyframes.resize(numKeyframes);

  for (auto &keyframe : animation.data.keyframes) {
    file.read(keyframe.target);
    file.read(keyframe.interpolation);
    file.read(keyframe.jointTarget);
    file.read(keyframe.joint);

    uint32_t numValues = 0;
    file.read(numValues);
    keyframe.keyframeTimes.resize(numValues);
    keyframe.keyframeValues.resize(numValues);
    file.read(keyframe.keyframeTimes);
    file.read(keyframe.keyframeValues);
  }

  return mRegistry.getAnimations().addAsset(animation);
}

std::filesystem::path
AssetManager::createPrefabFromAsset(const AssetData<PrefabAsset> &asset) {
  String extension = ".lqprefab";
  std::filesystem::path assetPath = (mAssetsPath / (asset.name + extension));
  OutputBinaryStream file(assetPath);

  LIQUID_ASSERT(file.good(), "File cannot be created for writing");

  AssetFileHeader header{};
  header.type = AssetType::Prefab;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  // Load assets
  uint32_t numMeshes = static_cast<uint32_t>(asset.data.meshes.size());
  file.write(numMeshes);

  for (auto &component : asset.data.meshes) {
    auto &mesh = mRegistry.getMeshes().getAsset(component.value);
    auto path = std::filesystem::relative(mesh.path, mAssetsPath).string();
    std::replace(path.begin(), path.end(), '\\', '/');
    file.write(path);
  }

  uint32_t numSkinnedMeshes =
      static_cast<uint32_t>(asset.data.skinnedMeshes.size());
  file.write(numSkinnedMeshes);

  for (auto &component : asset.data.skinnedMeshes) {
    auto &mesh = mRegistry.getSkinnedMeshes().getAsset(component.value);
    auto path = std::filesystem::relative(mesh.path, mAssetsPath).string();
    std::replace(path.begin(), path.end(), '\\', '/');
    file.write(path);
  }

  uint32_t numSkeletons = static_cast<uint32_t>(asset.data.skeletons.size());
  file.write(numSkeletons);

  for (auto &component : asset.data.skeletons) {
    auto &skeleton = mRegistry.getSkeletons().getAsset(component.value);
    auto path = std::filesystem::relative(skeleton.path, mAssetsPath).string();
    std::replace(path.begin(), path.end(), '\\', '/');
    file.write(path);
  }

  uint32_t numAnimations = 0;
  for (auto &animator : asset.data.animators) {
    numAnimations += static_cast<uint32_t>(animator.value.animations.size());
  }
  file.write(numAnimations);

  std::map<AnimationAssetHandle, uint32_t> localMap;
  uint32_t lastId = 0;

  for (auto &component : asset.data.animators) {
    for (auto &handle : component.value.animations) {
      auto &animation = mRegistry.getAnimations().getAsset(handle);
      auto path =
          std::filesystem::relative(animation.path, mAssetsPath).string();
      std::replace(path.begin(), path.end(), '\\', '/');
      file.write(path);

      if (localMap.find(handle) == localMap.end()) {
        localMap.insert_or_assign(handle, lastId++);
      }
    }
  }

  // Load component data
  uint32_t dummySize = 0;
  // Size for transforms
  file.write(dummySize);

  // Size for meshes
  file.write(dummySize);

  // Size for skinned meshes
  file.write(dummySize);

  // Size for skeletons
  file.write(dummySize);

  auto numAnimators = static_cast<uint32_t>(asset.data.animators.size());
  file.write(numAnimators);

  for (auto &animator : asset.data.animators) {
    file.write(animator.entity);

    auto numAnimations =
        static_cast<uint32_t>(animator.value.animations.size());
    file.write(numAnimations);
    for (auto &handle : animator.value.animations) {
      file.write(localMap.at(handle));
    }
  }

  return assetPath;
}

PrefabAssetHandle
AssetManager::loadPrefabFromFile(const std::filesystem::path &filePath) {
  InputBinaryStream file(filePath);

  AssetFileHeader header;
  String magic(ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), ASSET_FILE_MAGIC_LENGTH);
  file.read(header.version);
  file.read(header.type);

  LIQUID_ASSERT(magic == header.magic, "Data is not a liquid file");
  LIQUID_ASSERT(header.type == AssetType::Prefab, "File is not an prefab");

  AssetData<PrefabAsset> prefab{};
  prefab.path = filePath;
  prefab.name = filePath.filename().string();
  prefab.type = header.type;

  {
    auto &map = mRegistry.getMeshes();
    auto &component = prefab.data.meshes;

    uint32_t numAssets = 0;
    file.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);
    component.resize(numAssets);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      std::filesystem::path actualPath =
          (mAssetsPath / actual.at(i)).make_preferred();

      for (auto &[handle, asset] : map.getAssets()) {
        if (asset.path == actualPath) {
          component.at(i).value = handle;
          break;
        }
      }
    }
  }

  {
    auto &map = mRegistry.getSkinnedMeshes();
    auto &component = prefab.data.skinnedMeshes;

    uint32_t numAssets = 0;
    file.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);
    component.resize(numAssets);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      std::filesystem::path actualPath =
          (mAssetsPath / actual.at(i)).make_preferred();

      for (auto &[handle, asset] : map.getAssets()) {
        if (asset.path == actualPath) {
          component.at(i).value = handle;
          break;
        }
      }
    }
  }

  {
    auto &map = mRegistry.getSkeletons();
    auto &component = prefab.data.skeletons;

    uint32_t numAssets = 0;
    file.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);
    component.resize(numAssets);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      std::filesystem::path actualPath =
          (mAssetsPath / actual.at(i)).make_preferred();

      for (auto &[handle, asset] : map.getAssets()) {
        if (asset.path == actualPath) {
          component.at(i).value = handle;
          break;
        }
      }
    }
  }

  std::vector<AnimationAssetHandle> localMap;
  {
    auto &map = mRegistry.getAnimations();

    uint32_t numAssets = 0;
    file.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);
    localMap.resize(numAssets);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      std::filesystem::path actualPath =
          (mAssetsPath / actual.at(i)).make_preferred();

      for (auto &[handle, asset] : map.getAssets()) {
        if (asset.path == actualPath) {
          localMap.at(i) = handle;
          break;
        }
      }
    }
  }

  uint32_t dummy = 0;
  file.read(dummy);
  file.read(dummy);
  file.read(dummy);
  file.read(dummy);

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    auto &map = mRegistry.getAnimations();

    prefab.data.animators.resize(numComponents);

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      file.read(entity);

      uint32_t numAnimations = 0;
      file.read(numAnimations);

      std::vector<uint32_t> animations(numAnimations);
      file.read(animations);

      prefab.data.animators.at(i).value.animations.resize(numAnimations);

      for (size_t j = 0; j < animations.size(); ++j) {
        prefab.data.animators.at(i).value.animations.at(j) =
            localMap.at(animations.at(j));
      }
    }
  }

  return mRegistry.getPrefabs().addAsset(prefab);
}

} // namespace liquid
