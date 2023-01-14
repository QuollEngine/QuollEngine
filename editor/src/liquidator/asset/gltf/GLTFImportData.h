#pragma once

#include "liquid/asset/AssetCache.h"

#include "liquidator/asset/gltf/TinyGLTF.h"

namespace liquidator {

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
  GLTFToAsset<liquid::SkeletonAssetHandle> skeletonMap;
};

/**
 * @brief Transient animation data
 *
 * Used to store GLTF animations with engine
 * specific animation handles
 */
struct AnimationData {
  /**
   * Node to animation map
   */
  std::map<uint32_t, std::vector<liquid::AnimationAssetHandle>>
      nodeAnimationMap;

  /**
   * Skin to animation map
   */
  std::map<uint32_t, std::vector<liquid::AnimationAssetHandle>>
      skinAnimationMap;
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
  liquid::AssetCache &assetCache;

  /**
   * Target path
   */
  liquid::Path targetPath;

  /**
   * @brief GLTF model
   */
  const tinygltf::Model &model;

  /**
   * @brief Warnings
   */
  std::vector<liquid::String> warnings;

  /**
   * @brief Texture map
   */
  GLTFToAsset<liquid::TextureAssetHandle> textures;

  /**
   * @brief Material map
   */
  GLTFToAsset<liquid::MaterialAssetHandle> materials;

  /**
   * @brief Skeleton data
   */
  SkeletonData skeletons;

  /**
   * @brief Animation data
   */
  AnimationData animations;

  /**
   * @brief Mesh map
   */
  GLTFToAsset<liquid::MeshAssetHandle> meshes;

  /**
   * @brief Skinned mesh map
   */
  GLTFToAsset<liquid::SkinnedMeshAssetHandle> skinnedMeshes;

  /**
   * @brief Output path
   */
  liquid::Result<liquid::Path> outputPath =
      liquid::Result<liquid::Path>::Error("Empty");
};

} // namespace liquidator
