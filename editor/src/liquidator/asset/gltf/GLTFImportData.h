#pragma once

#include "quoll/asset/AssetCache.h"

#include "liquidator/asset/ImageLoader.h"
#include "liquidator/asset/gltf/TinyGLTF.h"
#include "liquidator/asset/UUIDMap.h"

namespace quoll::editor {

/**
 * @brief Mapping for GLTF index to
 * engine specific asset
 *
 * @tparam THandle Asset handle
 */
template <class THandle> struct GLTFToAsset {
  /**
   * GLTF index to asset handle map
   */
  std::map<size_t, THandle> map;
};

/**
 * @brief Transient skeleton data
 *
 * Used to store GLTF skins an joints with engine
 * specific skeleton handles
 */
struct SkeletonData {
  /**
   * GLTF joint to engine specific joint I
   */
  std::unordered_map<uint32_t, uint32_t> gltfToNormalizedJointMap;

  /**
   * Joints that are associated with skins
   */
  std::unordered_map<uint32_t, uint32_t> jointSkinMap;

  /**
   * Skin map
   */
  GLTFToAsset<SkeletonAssetHandle> skeletonMap;
};

/**
 * @brief Transient animation data
 *
 * Used to store GLTF animations with engine
 * specific animation handles
 */
struct AnimationData {
  /**
   * Node to animator map
   */
  std::map<uint32_t, AnimatorAssetHandle> nodeAnimatorMap;

  /**
   * Skin to animator map
   */
  std::map<uint32_t, AnimatorAssetHandle> skinAnimatorMap;
};

/**
 * @brief GLTF import data
 *
 * Stores all the information to perform the import
 */
struct GLTFImportData {
  /**
   * Asset cache
   */
  AssetCache &assetCache;

  /**
   * Image loader
   */
  ImageLoader &imageLoader;

  /**
   * Source asset path
   */
  Path sourcePath;

  /**
   * Uuid map
   */
  UUIDMap uuids;

  /**
   * GLTF model
   */
  const tinygltf::Model &model;

  /**
   * Optimize data
   */
  bool optimize = false;

  /**
   * Warnings
   */
  std::vector<String> warnings;

  /**
   * Texture map
   */
  GLTFToAsset<TextureAssetHandle> textures;

  /**
   * Material map
   */
  GLTFToAsset<MaterialAssetHandle> materials;

  /**
   * Skeleton data
   */
  SkeletonData skeletons;

  /**
   * Animation data
   */
  AnimationData animations;

  /**
   * Mesh map
   */
  GLTFToAsset<MeshAssetHandle> meshes;

  /**
   * Map of materials associated
   * with meshes
   */
  std::map<MeshAssetHandle, std::vector<MaterialAssetHandle>> meshMaterials;

  /**
   * Directional light map
   */
  GLTFToAsset<DirectionalLight> directionalLights;

  /**
   * Point light map
   */
  GLTFToAsset<PointLight> pointLights;

  /**
   * Output path
   */
  Result<Path> outputPath = Result<Path>::Error("Empty");

  /**
   * Output uuids
   */
  UUIDMap outputUuids;
};

/**
 * @brief Get uuid for object
 *
 * @param importData Import data
 * @param name Asset name
 * @return Uuid
 */
static Uuid getOrCreateGLTFUuid(const GLTFImportData &importData,
                                const String &name) {
  return getOrCreateUuidFromMap(importData.uuids, name);
}

/**
 * @brief Get GLTF asset name
 *
 * @param importData Import data
 * @param name Item name
 * @return Asset name
 */
static String getGLTFAssetName(const GLTFImportData &importData,
                               const String &name) {
  return importData.sourcePath.filename().string() + "/" + name;
}

} // namespace quoll::editor
