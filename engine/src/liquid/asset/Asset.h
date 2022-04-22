#pragma once

namespace liquid {

enum class MaterialAssetHandle : uint32_t { Invalid = 0 };

enum class TextureAssetHandle : uint32_t { Invalid = 0 };

enum class MeshAssetHandle : uint32_t { Invalid = 0 };

enum class SkinnedMeshAssetHandle : uint32_t { Invalid = 0 };

enum class SkeletonAssetHandle : uint32_t { Invalid = 0 };

enum class AnimationAssetHandle : uint32_t { Invalid = 0 };

enum class PrefabAssetHandle : uint32_t { Invalid = 0 };

enum class LuaScriptAssetHandle : uint32_t { Invalid = 0 };

enum class AssetType : uint8_t {
  None,
  Material,
  Texture,
  Mesh,
  SkinnedMesh,
  Skeleton,
  Animation,
  Prefab,
  LuaScript
};

inline const String getAssetTypeString(AssetType type) {
  switch (type) {
  case AssetType::Material:
    return "material";
  case AssetType::Texture:
    return "texture";
  case AssetType::Mesh:
    return "mesh";
  case AssetType::SkinnedMesh:
    return "skinned mesh";
  case AssetType::Animation:
    return "animation";
  case AssetType::Prefab:
    return "prefab";
  case AssetType::LuaScript:
    return "lua script";
  default:
    return "none";
  }
}

} // namespace liquid
