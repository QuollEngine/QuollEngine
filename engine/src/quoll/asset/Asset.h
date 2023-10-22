#pragma once

namespace quoll {

enum class MaterialAssetHandle : u32 { Null = 0 };

enum class TextureAssetHandle : u32 { Null = 0 };

enum class FontAssetHandle : u32 { Null = 0 };

enum class MeshAssetHandle : u32 { Null = 0 };

enum class SkeletonAssetHandle : u32 { Null = 0 };

enum class AnimationAssetHandle : u32 { Null = 0 };

enum class AnimatorAssetHandle : u32 { Null = 0 };

enum class AudioAssetHandle : u32 { Null = 0 };

enum class PrefabAssetHandle : u32 { Null = 0 };

enum class LuaScriptAssetHandle : u32 { Null = 0 };

enum class EnvironmentAssetHandle : u32 { Null = 0 };

enum class SceneAssetHandle : u32 { Null = 0 };

enum class InputMapAssetHandle : u32 { Null = 0 };

enum class AssetType : u8 {
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
  Animator,
  Scene,
  InputMap
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
  case AssetType::Scene:
    return "scene";
  case AssetType::InputMap:
    return "input map";
  default:
    return "none";
  }
}

} // namespace quoll
