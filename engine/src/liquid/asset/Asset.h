#pragma once

namespace liquid {

enum class MaterialAssetHandle : uint32_t { Null = 0 };

enum class TextureAssetHandle : uint32_t { Null = 0 };

enum class FontAssetHandle : uint32_t { Null = 0 };

enum class MeshAssetHandle : uint32_t { Null = 0 };

enum class SkeletonAssetHandle : uint32_t { Null = 0 };

enum class AnimationAssetHandle : uint32_t { Null = 0 };

enum class AnimatorAssetHandle : uint32_t { Null = 0 };

enum class AudioAssetHandle : uint32_t { Null = 0 };

enum class PrefabAssetHandle : uint32_t { Null = 0 };

enum class LuaScriptAssetHandle : uint32_t { Null = 0 };

enum class EnvironmentAssetHandle : uint32_t { Null = 0 };

enum class AssetType : uint8_t {
  None,
  Material,
  Texture,
  Mesh,
  SkinnedMesh,
  Skeleton,
  Animation,
  Audio,
  Prefab,
  LuaScript,
  Font,
  Environment,
  Animator
};

inline const String getAssetTypeString(AssetType type) {
  switch (type) {
  case AssetType::Material:
    return "material";
  case AssetType::Texture:
    return "texture";
  case AssetType::Font:
    return "font";
  case AssetType::Mesh:
    return "mesh";
  case AssetType::SkinnedMesh:
    return "skinned mesh";
  case AssetType::Animation:
    return "animation";
  case AssetType::Animator:
    return "animator";
  case AssetType::Environment:
    return "environment";
  case AssetType::Audio:
    return "audio";
  case AssetType::Prefab:
    return "prefab";
  case AssetType::LuaScript:
    return "lua script";
  default:
    return "none";
  }
}

} // namespace liquid
