#pragma once

namespace liquid {

struct PrefabAssetItem {
  MeshAssetHandle mesh = MeshAssetHandle::Invalid;
  SkinnedMeshAssetHandle skinnedMesh = SkinnedMeshAssetHandle::Invalid;
  SkeletonAssetHandle skeleton = SkeletonAssetHandle::Invalid;
  std::vector<AnimationAssetHandle> animations;
};

struct PrefabAsset {
  std::vector<PrefabAssetItem> items;
};

} // namespace liquid
