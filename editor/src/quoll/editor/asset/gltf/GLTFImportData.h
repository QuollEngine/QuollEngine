#pragma once

#include "quoll/asset/AssetCache.h"
#include "quoll/editor/asset/ImageLoader.h"
#include "quoll/editor/asset/UUIDMap.h"
#include "quoll/editor/asset/gltf/TinyGLTF.h"

namespace quoll::editor {

template <class THandle> struct GLTFToAsset {
  std::map<usize, THandle> map;
};

struct SkeletonData {
  std::unordered_map<u32, u32> gltfToNormalizedJointMap;

  std::unordered_map<u32, u32> jointSkinMap;

  GLTFToAsset<AssetHandle<SkeletonAsset>> skeletonMap;
};

struct AnimationData {
  std::map<u32, AssetHandle<AnimatorAsset>> nodeAnimatorMap;

  std::map<u32, AssetHandle<AnimatorAsset>> skinAnimatorMap;
};

struct GLTFImportData {
  AssetCache &assetCache;

  ImageLoader &imageLoader;

  Path sourcePath;

  UUIDMap uuids;

  const tinygltf::Model &model;

  bool optimize = false;

  std::vector<String> warnings;

  GLTFToAsset<AssetHandle<TextureAsset>> textures;

  GLTFToAsset<AssetHandle<MaterialAsset>> materials;

  SkeletonData skeletons;

  AnimationData animations;

  GLTFToAsset<AssetHandle<MeshAsset>> meshes;

  std::map<AssetHandle<MeshAsset>, std::vector<AssetHandle<MaterialAsset>>>
      meshMaterials;

  GLTFToAsset<DirectionalLight> directionalLights;

  GLTFToAsset<PointLight> pointLights;

  Result<Path> outputPath = Error("Empty");

  UUIDMap outputUuids;
};

static Uuid getOrCreateGLTFUuid(const GLTFImportData &importData,
                                const String &name) {
  return getOrCreateUuidFromMap(importData.uuids, name);
}

static String getGLTFAssetName(const GLTFImportData &importData,
                               const String &name) {
  return importData.sourcePath.filename().string() + "/" + name;
}

} // namespace quoll::editor
